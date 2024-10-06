#include <future>

#include "Pathfinder.hpp"
#include "TrainManager.hpp"

bool Pathfinder::findBranch(Node* rootNode, Node* target, NodeBranch& path, NodeBranch& branch) {
    if (!rootNode) return false;
    path.push_back(rootNode);

    if (rootNode == target || rootNode->status == Failure) {
        branch = path;
        return true;
    }

    if (findBranch(rootNode->left.get(), target, path, branch) || findBranch(rootNode->right.get(), target, path, branch)) {
        return true;
    }

    path.pop_back();
    return false;
}

void Pathfinder::generateBranch(NodeBranch& currentBranch, const bool direction, const size_t depth) {
    if (depth == 0) return;
    const auto lastNode = currentBranch.back();
    if (direction) {
        lastNode->right = std::make_unique<Node>(direction);
        currentBranch.push_back(lastNode->right.get());
        return generateBranch(currentBranch, direction, depth - 1);
    }
    lastNode->left = std::make_unique<Node>(direction);
    currentBranch.push_back(lastNode->left.get());
    generateBranch(currentBranch, direction, depth - 1);
}

void Pathfinder::markFailed(NodeBranch& nodes, const bool check, const int index) {
    size_t arrayIndex;
    if (index == -1) arrayIndex = nodes.size() - 1;
    else arrayIndex = index;
    const auto lastNode = nodes.at(arrayIndex);
    if (check) if (const auto parentNode = nodes.at(arrayIndex - 1); parentNode->left && parentNode->left->status == Failure && lastNode->right && lastNode->right->status == Failure) {
        parentNode->status = Failure;
        parentNode->left.reset();
        parentNode->right.reset();
        if (parentNode->left.get() != lastNode && parentNode->right.get() != lastNode) {
            log::error("Invalid node branch!");
            std::abort();
        }
        nodes.pop_back();
    } else {
        lastNode->status = Failure;
        pruneAfterIndex(nodes, arrayIndex);
    }
    markFailed(nodes, true, -1);
}

void Pathfinder::pruneAfterIndex(NodeBranch& nodes, const size_t index) {
    if (index >= nodes.size()) {
        log::error("Invalid index!");
        std::abort();
    }
    const auto lastNode = nodes.at(index);
    lastNode->left.reset();
    lastNode->right.reset();
    nodes.erase(nodes.begin() + index + 1, nodes.end());
}

void Pathfinder::setBranchSuccess(const NodeBranch& nodes, const bool completion) {
    for (const auto node : nodes) {
        if (completion && node == nodes.back()) {
            node->status = Completion;
        } else if (node->status != Failure)
        node->status = Success;
    }
}

std::pair<NodeBranch&, InstanceResult&> Pathfinder::analyseResults(std::vector<InstanceResult>& results) {
    std::vector<NodeBranch*> branches;
    bool completion = false;
    for (auto& result : results) {
        if (result.status == DeadAfterInstructions || result.status == CompletedAfterInstructions) {
            const auto remaining = result.index - (result.branch.size() - 1);
            generateBranch(result.branch, result.action, remaining);
            if (result.status == DeadAfterInstructions) {
                markFailed(result.branch, false, -1);
                setBranchSuccess(result.branch, false);
            } else {
                completion = true;
                setBranchSuccess(result.branch, true);
            }
        } else if (result.status == Dead) {
            markFailed(result.branch, false, result.index);
            setBranchSuccess(result.branch, false);
        } else if (result.status == Completed) {
            pruneAfterIndex(result.branch, result.index);
            completion = true;
            setBranchSuccess(result.branch, true);
        }
        branches.push_back(&result.branch);
    }

    if (completion) {
        for (size_t i = 0; i < results.size(); i++) {
            if (const auto result = results.at(i); result.status == DeadAfterInstructions || result.status == Dead) {
                results.erase(results.begin() + i);
                branches.erase(branches.begin() + i);
            }
        }
    }

    std::ranges::sort(branches, [](const NodeBranch* a, const NodeBranch* b) {
        return a->size() < b->size();
    });

    return {*branches.back(), results.back()};
}

template<typename T>
bool isStartOf(const std::vector<T>& start, const std::vector<T>& full) {
    return start.size() <= full.size() &&
           std::equal(start.begin(), start.end(), full.begin());
}

void Pathfinder::sanitiseBranch(std::vector<NodeBranch>& branch) {
    for (size_t i = 0; i < branch.size(); i++) {
        for (size_t j = 0; j < branch.size(); j++) {
            if (i == j) continue;
            if (isStartOf(branch.at(i), branch.at(j))) {
                branch.erase(branch.begin() + i);
                i--;
                break;
            }
        }
    }
}

void Pathfinder::generatePaths(Node* node, NodeBranch& currentBranch, std::vector<NodeBranch>& branches, const size_t depth) {
    if (node == nullptr || depth == 0) {
        branches.push_back(currentBranch);
        return;
    }

    if (node->status == Failure) {
        branches.push_back(currentBranch);
        return;
    }

    if (!node->left) {
        node->left = std::make_unique<Node>(false);
    }
    if (!node->right) {
        node->right = std::make_unique<Node>(true);
    }

    currentBranch.push_back(node->left.get());
    generatePaths(node->left.get(), currentBranch, branches, depth - 1);
    currentBranch.pop_back();

    currentBranch.push_back(node->right.get());
    generatePaths(node->right.get(), currentBranch, branches, depth - 1);
    currentBranch.pop_back();
}

NodeBranch& Pathfinder::findPath(Node* root) {
    std::vector<NodeBranch> branches;
    NodeBranch currentBranch;
    generatePaths(root, currentBranch, branches, n);
    sanitiseBranch(branches);

    std::promise<std::vector<InstanceResult>> resultsPromise;
    auto resultsFuture = resultsPromise.get_future();

    manager->assignTasks(branches, [&resultsPromise](std::vector<InstanceResult> results) {
        resultsPromise.set_value(std::move(results));
    });

    auto rawResults = resultsFuture.get();

    auto [branch, result] = analyseResults(rawResults);

    if (result.status == DeadAfterInstructions || result.status == Dead) {
        const auto rootNode = branch.at(branch.size() - 2);
        if (rootNode->left && !rootNode->right) {
            rootNode->right = std::make_unique<Node>(true);
            return findPath(rootNode->right.get());
        }
        if (rootNode->right && !rootNode->left) {
            rootNode->left = std::make_unique<Node>(false);
            return findPath(rootNode->left.get());
        }
        std::abort(); // if it gets here we're cooked regardless
    }
    return branch;
}

NodeBranch& Pathfinder::startSearch() {
    return findPath(root.get());
}