#ifndef INSTANCERESULT_HPP
#define INSTANCERESULT_HPP

#include <utility>

#include "Node.hpp"

using namespace geode::prelude;

enum LevelStatus {
    Dead,
    Completed,
};

struct InstanceResult {
    LevelStatus status;
    size_t index;
    NodeBranch branch;
    bool action;
    bool afterInstructions;

    InstanceResult(const LevelStatus status, const size_t index, NodeBranch branch, const bool action, const bool afterInstructions)
        : status(status), index(index), branch(std::move(branch)), action(action), afterInstructions(afterInstructions) {
        log::info("{}", afterInstructions);
    }
};

#endif