#ifndef INSTANCERESULT_HPP
#define INSTANCERESULT_HPP

#include <utility>

#include "Node.hpp"

enum LevelStatus {
    Dead,
    Completed,
};

struct InstanceResult {
    LevelStatus status;
    size_t index;
    NodeBranch branch;
    bool action = false;
    bool afterInstructions;

    InstanceResult(const LevelStatus status, const size_t index, NodeBranch branch, const bool afterInstructions)
        : status(status), index(index), branch(std::move(branch)), afterInstructions() {}
};

#endif