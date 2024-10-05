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
    std::unique_ptr<NodeBranch> branch;
    bool action = false;

    explicit InstanceResult(const LevelStatus status, const size_t index, const NodeBranch& branch)
        : status(status), index(index), branch(std::make_unique<NodeBranch>(branch)) {}
};

#endif