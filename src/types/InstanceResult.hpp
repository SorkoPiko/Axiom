#ifndef INSTANCERESULT_HPP
#define INSTANCERESULT_HPP

#include <utility>

#include "Node.hpp"

enum LevelStatus {
    Dead,
    DeadAfterInstructions,
    Completed,
    CompletedAfterInstructions
};

struct InstanceResult {
    LevelStatus status;
    size_t index;
    NodeBranch branch;
    bool action = false;

    InstanceResult(const LevelStatus status, const size_t index, NodeBranch branch)
        : status(status), index(index), branch(std::move(branch)) {}
};

#endif