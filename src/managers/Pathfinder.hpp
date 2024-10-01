#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <memory>
#include <vector>
#include "../types/Node.hpp"

class TrainManager;

class Pathfinder {
    std::unique_ptr<Node> root;
    TrainManager* trainManager;
    size_t n;

    void generatePaths(const Node* node, std::vector<bool>& currentPath, std::vector<std::vector<bool>>& paths, size_t depth);
    static bool explorePaths(Node* node, const std::vector<std::vector<bool>>& paths, const std::vector<std::pair<bool, size_t>>& results);
    static std::vector<bool> findSuccessfulPathFromNode(Node* node);

public:
    Pathfinder(
        TrainManager* trainManager,
        const size_t depth
    ) : root(std::make_unique<Node>(false)), trainManager(trainManager), n(depth) {}

    std::vector<bool> findSuccessfulPath();
};

#endif