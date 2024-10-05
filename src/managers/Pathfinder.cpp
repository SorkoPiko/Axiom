#include "Pathfinder.hpp"

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

NodeBranch& Pathfinder::analyseResults(const std::vector<InstanceResult> &results) {
    std::vector<NodeBranch*> branches;
    bool completion = false;
    for (const auto& result : results) {
        if (result.status == DeadAfterInstructions || result.status == CompletedAfterInstructions) {
            const auto remaining = result.index - (result.branch->size() - 1);
            generateBranch(*result.branch, result.action, remaining);
            if (result.status == DeadAfterInstructions) {
                markFailed(*result.branch, false, -1);
                setBranchSuccess(*result.branch, false);
            } else {
                completion = true;
                setBranchSuccess(*result.branch, true);
            }
        } else if (result.status == Dead) {
            markFailed(*result.branch, false, result.index);
            setBranchSuccess(*result.branch, false);
        } else if (result.status == Completed) {
            pruneAfterIndex(*result.branch, result.index);
            completion = true;
            setBranchSuccess(*result.branch, true);
        }
        branches.push_back(result.branch.get());
    }

    if (completion) {
        std::erase_if(branches, [](const Node& node) {
            return node.status != Completed && node.status != CompletedAfterInstructions;
        });
    }

    std::ranges::sort(branches, [](const NodeBranch* a, const NodeBranch* b) {
        return a->size() < b->size();
    });

    return *branches.back();
}

void Pathfinder::generatePaths(Node* node, std::vector<bool>& currentPath, std::vector<std::vector<bool>>& paths, const size_t depth) {
    if (node == nullptr || depth == n) {
        paths.push_back(currentPath);
        return;
    }

    if (node->status == Failure) {
        currentPath.clear();
        paths.push_back(currentPath);
        return;
    }

    if (!node->left) {
        node->left = std::make_unique<Node>(false);
    }
    if (!node->right) {
        node->right = std::make_unique<Node>(true);
    }

    currentPath.push_back(false);
    generatePaths(node->left.get(), currentPath, paths, depth + 1);
    currentPath.back() = true;
    generatePaths(node->right.get(), currentPath, paths, depth + 1);
    currentPath.pop_back();
}