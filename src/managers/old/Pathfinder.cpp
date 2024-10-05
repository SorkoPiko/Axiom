// this code is really bad so please don't read it
// if you try to read it you will probably just end up really confused

#include <vector>
#include <memory>
#include <future>
#include <algorithm>
#include <Geode/loader/Log.hpp>

#include "../TrainManager.hpp"
#include "Pathfinder.hpp"

using namespace geode::prelude;

void Pathfinder::nodeFailed(std::vector<Node*>& nodes) {
    std::erase_if(nodes, [](const Node* node) {
        return node == nullptr;
    });

    if (nodes.empty()) {
        return;
    }

    for (const auto& node : nodes) {
        auto address = reinterpret_cast<uintptr_t>(node);
        log::info("NODEFAILED CALL {}", address);
    }

    Node* lastNode = nodes.back();
    if (lastNode == nullptr) {
        return;
    }

    auto address = reinterpret_cast<uintptr_t>(lastNode);
    log::info("marking {} as failed", address);
    lastNode->status = Failure;
    lastNode->left = nullptr;
    lastNode->right = nullptr;
    nodes.pop_back();

    if (!nodes.empty()) {
        checkChildNodes(nodes);
    }
}

void Pathfinder::checkChildNodes(std::vector<Node*>& nodes) {
    if (nodes.back()->left && nodes.back()->left->status == Failure && nodes.back()->right && nodes.back()->right->status == Failure) {
        log::info("both child nodes are failed");
        nodeFailed(nodes);
    }
}

void Pathfinder::generatePaths(Node* node, std::vector<bool>& currentPath, std::vector<std::vector<bool>>& paths, size_t depth) {
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

std::vector<Node*> Pathfinder::findNodeBranch(Node* root, const Node* target) {
    std::vector<Node*> branch;
    std::vector<Node*> path;

    std::function<bool(Node*)> dfs = [&](Node* currentNode) -> bool {
        if (!currentNode) return false;
        path.push_back(currentNode);

        if (currentNode == target) {
            branch = path;
            return true;
        }

        if (dfs(currentNode->left.get()) || dfs(currentNode->right.get())) {
            return true;
        }

        path.pop_back();
        return false;
    };

    dfs(root);
    return branch;
}

void cleanNodeVector(std::vector<Node*>& nodes) {
    if (const auto it = std::ranges::find_if(nodes, [](const Node* node) {
        return node->status == Failure;
    }); it != nodes.end()) {
        nodes.erase(it, nodes.end());
    }
}

int countDifference(std::vector<Node*>& nodes) {
    bool currentAction = nodes.front()->input;
    return std::ranges::count_if(nodes, [currentAction](const Node* node) mutable {
        const auto result = node->input != currentAction;
        currentAction = node->input;
        return result;
    });
}

std::pair<std::vector<Node *>, bool> Pathfinder::analyseResults(Node *rootNode, const std::vector<std::vector<bool>> &paths, const std::vector<std::pair<bool, size_t>> &results) {
    std::vector<std::vector<Node*>> nodePaths;

    for (size_t i = 0; i < paths.size(); i++) {
        auto path = paths[i];
        const auto [success, failIndex] = results[i];
        log::info("{}", path);
        std::vector<Node*> emptyNodesa;
        auto nodeInstructionsa = findNodesFromInstructions(rootNode, path, emptyNodesa);
        for (const auto& node : nodeInstructionsa) {
            auto address = reinterpret_cast<uintptr_t>(node);
            log::info("{}", address);
        }

        if (failIndex >= path.size()) {
            log::info("Fail index {} is greater than paths size {}", failIndex, path.size() - 1);
            const auto remaining = failIndex - (path.size() - 1);
            std::vector<Node*> emptyNodes;
            auto nodeInstructions = findNodesFromInstructions(rootNode, path, emptyNodes);
            if (nodeInstructions.empty()) {
                log::info("Invalid path: {}", path);
                //continue;
            }
            auto newNode = nodeInstructions.back();
            const auto node = generatePathsDown(newNode, path.back(), remaining);
            auto address = reinterpret_cast<uintptr_t>(node);
            log::info("GENPATHSDOWN {}", address);
            auto nodePath = findNodeBranch(rootNode, node);
            auto nodePathCopy = nodePath;
            if (success) { // Finished the level
                node->status = Completion;
            } else {
                nodeFailed(nodePathCopy);
            }
            cleanNodeVector(nodePath);
            nodePaths.push_back(nodePath);
            continue;
        }

        Node* currentNode = rootNode;

        auto pathCopy = path;
        pathCopy.erase(pathCopy.begin() + failIndex, pathCopy.end());
        std::vector<Node*> emptyNodes;
        auto nodes = findNodesFromInstructions(currentNode, pathCopy, emptyNodes);
        auto nodesCopy = nodes;



        if (success) { // Finished the level
            nodes.back()->status = Completion;
        } else {
            nodeFailed(nodesCopy);
        }
        cleanNodeVector(nodes);
        nodePaths.push_back(nodes);
    }

    std::map<std::vector<Node*>, int> nodeLengthMap;
    int maxLength = 0;

    for (auto& nodePath : nodePaths) {
        if (nodePath.size() > maxLength) {
            maxLength = nodePath.size();
        }
        nodeLengthMap[nodePath] = nodePath.size();
    }

    std::vector<std::vector<Node*>> longestNodes;

    for (auto& [nodePath, length] : nodeLengthMap) {
        if (length == maxLength) {
            longestNodes.push_back(nodePath);
        }
    }

    std::map<std::vector<Node*>, int> nodeDifferenceMap;
    int minDifference = INT_MAX;

    for (auto& nodePath : longestNodes) {
        if (const auto difference = countDifference(nodePath); difference < minDifference) {
            minDifference = difference;
        }
        nodeDifferenceMap[nodePath] = countDifference(nodePath);
    }

    for (auto& [nodePath, difference] : nodeDifferenceMap) {
        if (difference == minDifference) {
            return {nodePath, nodePath.back()->status == Completion};
        }
    }

    return {};

    // std::queue<std::pair<Node*, size_t>> nodeQueue;
    // nodeQueue.emplace(node, 0);
    //
    // size_t pathIndex = 0;
    // while (!nodeQueue.empty() && pathIndex < paths.size()) {
    //     auto [currentNode, depth] = nodeQueue.front();
    //     nodeQueue.pop();
    //
    //     if (depth == results[pathIndex].second) {
    //         currentNode->success = results[pathIndex].first;
    //         pathIndex++;
    //         continue;
    //     }
    //
    //     const bool nextInput = paths[pathIndex][depth];
    //     if (nextInput) {
    //         if (!currentNode->right) {
    //             currentNode->right = std::make_unique<Node>(true);
    //         }
    //         nodeQueue.emplace(currentNode->right.get(), depth + 1);
    //     } else {
    //         if (!currentNode->left) {
    //             currentNode->left = std::make_unique<Node>(false);
    //         }
    //         nodeQueue.emplace(currentNode->left.get(), depth + 1);
    //     }
    // }
    //
    // return std::ranges::any_of(results, [](const auto& result) { return result.first; });
}

std::vector<Node*> Pathfinder::findNodesFromInstructions(Node* currentNode, const std::vector<bool>& instructions, std::vector<Node*>& nodes) {
    for (const auto& instruction : instructions) {
        if (instruction) {
            if (!currentNode->right) {
                return {};
            }
            currentNode = currentNode->right.get();
        } else {
            if (!currentNode->left) {
                return {};
            }
            currentNode = currentNode->left.get();
        }
        if (currentNode->status == Failure) {
            break;
        }
        nodes.push_back(currentNode);
    }
    return nodes;
}

Node* Pathfinder::generatePathsDown(Node* node, bool input, size_t remaining) {
    if (remaining > 1000) {
        log::error("Very large remaining value: {}", remaining);
        std::abort();
    }
    if (remaining == 0) {
        return node;
    }

    if (input) {
        if (!node->right) {
            node->right = std::make_unique<Node>(input);
        }
        return generatePathsDown(node->right.get(), input, remaining - 1);
    }
    if (!node->left) {
        node->left = std::make_unique<Node>(input);
    }
    return generatePathsDown(node->left.get(), input, remaining - 1);
}

std::vector<bool> Pathfinder::findSuccessfulPath() {
    std::vector<std::vector<bool>> paths;
    std::vector<bool> currentPath;
    generatePaths(root.get(), currentPath, paths, 0);

    log::info("Generated paths");

    for (const auto& path : paths) {
        log::info("path: {}", path);
    }

    std::promise<std::vector<std::pair<bool, size_t>>> resultsPromise;
    auto resultsFuture = resultsPromise.get_future();

    trainManager->assignTasks(paths, [&resultsPromise](std::vector<std::pair<bool, size_t>> results) {
        resultsPromise.set_value(std::move(results));
    });

    log::info("Got results");

    const std::vector<std::pair<bool, size_t>> results = resultsFuture.get();

    const auto [path, completion] = analyseResults(root.get(), paths, results);

    log::info("Analysed results");

    for (const auto& node : path) {
        log::info("Node: {}", node->input);
    }
    log::info("Completion: {}", completion);

    return {}; // Return empty path if no successful path found
}

void Pathfinder::test() {
    findSuccessfulPath();
}