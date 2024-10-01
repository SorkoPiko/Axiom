#include <vector>
#include <memory>
#include <queue>
#include <future>
#include <algorithm>

#include "TrainManager.hpp"
#include "Pathfinder.hpp"

void Pathfinder::generatePaths(const Node* node, std::vector<bool>& currentPath, std::vector<std::vector<bool>>& paths, size_t depth) {
    if (node == nullptr || depth == n) {
        paths.push_back(currentPath);
        return;
    }

    currentPath.push_back(false);
    generatePaths(node->left.get(), currentPath, paths, depth + 1);
    currentPath.back() = true;
    generatePaths(node->right.get(), currentPath, paths, depth + 1);
    currentPath.pop_back();
}

bool Pathfinder::explorePaths(Node* node, const std::vector<std::vector<bool>>& paths, const std::vector<std::pair<bool, size_t>>& results) {
    std::queue<std::pair<Node*, size_t>> nodeQueue;
    nodeQueue.emplace(node, 0);

    size_t pathIndex = 0;
    while (!nodeQueue.empty() && pathIndex < paths.size()) {
        auto [currentNode, depth] = nodeQueue.front();
        nodeQueue.pop();

        if (depth == results[pathIndex].second) {
            currentNode->success = results[pathIndex].first;
            pathIndex++;
            continue;
        }

        const bool nextInput = paths[pathIndex][depth];
        if (nextInput) {
            if (!currentNode->right) {
                currentNode->right = std::make_unique<Node>(true);
            }
            nodeQueue.emplace(currentNode->right.get(), depth + 1);
        } else {
            if (!currentNode->left) {
                currentNode->left = std::make_unique<Node>(false);
            }
            nodeQueue.emplace(currentNode->left.get(), depth + 1);
        }
    }

    return std::any_of(results.begin(), results.end(), [](const auto& result) { return result.first; });
}

std::vector<bool> Pathfinder::findSuccessfulPathFromNode(Node* node) {
    if (node == nullptr || !node->success) {
        return {};
    }

    if (node->left == nullptr && node->right == nullptr) {
        return {node->input};
    }

    std::vector<bool> leftPath = findSuccessfulPathFromNode(node->left.get());
    if (!leftPath.empty()) {
        leftPath.insert(leftPath.begin(), node->input);
        return leftPath;
    }

    std::vector<bool> rightPath = findSuccessfulPathFromNode(node->right.get());
    if (!rightPath.empty()) {
        rightPath.insert(rightPath.begin(), node->input);
        return rightPath;
    }

    return {};
}

std::vector<bool> Pathfinder::findSuccessfulPath() {
    std::vector<std::vector<bool>> paths;
    std::vector<bool> currentPath;
    generatePaths(root.get(), currentPath, paths, 0);

    std::promise<std::vector<std::pair<bool, size_t>>> resultsPromise;
    std::future<std::vector<std::pair<bool, size_t>>> resultsFuture = resultsPromise.get_future();

    trainManager->assignTask(paths, [&resultsPromise](std::vector<std::pair<bool, size_t>> results) {
        resultsPromise.set_value(std::move(results));
    });

    std::vector<std::pair<bool, size_t>> results = resultsFuture.get();

    if (explorePaths(root.get(), paths, results)) {
        return findSuccessfulPathFromNode(root.get());
    }
    return {}; // Return empty path if no successful path found
}