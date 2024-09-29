#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <future>

#include "TrainManager.hpp"
#include "../types/Node.hpp"

class Pathfinder {
    std::unique_ptr<Node> root;
    TrainManager* trainManager;

    bool explorePath(Node* node, std::vector<bool>& currentPath);

public:
    Pathfinder(TrainManager* trainManager)
        : root(std::make_unique<Node>(false)), trainManager(trainManager) {}

    std::vector<bool> findSuccessfulPath() {
        std::vector<bool> path;
        if (explorePath(root.get(), path)) {
            return path;
        }
        return {}; // no path found
    }
};

#endif
