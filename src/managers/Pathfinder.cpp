#include "Pathfinder.hpp"

bool Pathfinder::explorePath(Node* node, std::vector<bool>& currentPath) {
    if (node == nullptr) {
        return false;
    }

    currentPath.push_back(node->input);

    std::promise<std::pair<bool, size_t>> instancePromise;
    std::future<std::pair<bool, size_t>> instanceFuture = instancePromise.get_future();

    trainManager->assignTask(currentPath, [&instancePromise](bool success, size_t failIndex) {
        instancePromise.set_value({success, failIndex});
    });

    auto [success, failIndex] = instanceFuture.get();

    if (success) {
        node->success = true;
        return true;
    }

    if (failIndex == currentPath.size() - 1) {
        currentPath.pop_back();
        return false;
    }

    bool leftSuccess = false;
    bool rightSuccess = false;

    if (node->left == nullptr) {
        node->left = std::make_unique<Node>(false);
    }
    leftSuccess = explorePath(node->left.get(), currentPath);

    if (!leftSuccess) {
        if (node->right == nullptr) {
            node->right = std::make_unique<Node>(true);
        }
        rightSuccess = explorePath(node->right.get(), currentPath);
    }

    if (!leftSuccess && !rightSuccess) {
        node->success = false;
        node->left.reset();
        node->right.reset();
    }

    if (leftSuccess || rightSuccess) {
        return true;
    }

    currentPath.pop_back();
    return false;
}