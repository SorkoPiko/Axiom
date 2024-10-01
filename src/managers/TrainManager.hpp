#ifndef TRAINMANAGER_HPP
#define TRAINMANAGER_HPP

#include <unordered_map>
#include <vector>
#include <utility>

#include "Pathfinder.hpp"

class TrainManager {
    static TrainManager* instance;

    std::vector<PlayLayer*> instances;
    long long timewarp;
    GJGameLevel* level;
    Pathfinder* pathfinder;

    PlayLayer* addInstance();

    TrainManager(
        const long long timewarp,
        GJGameLevel* level
    ) : timewarp(timewarp), level(level) {}

public:
    static void assignTask(const std::vector<std::vector<bool>>& instructions, std::function<void(std::vector<std::pair<bool, size_t>>)> callback);

    bool checkInstance(const PlayLayer *instance) const;
    [[nodiscard]] long long getTimewarp() const;

    static TrainManager* create(GJGameLevel* level);
    static TrainManager* get();
};

#endif