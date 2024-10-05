#ifndef TRAINMANAGER_HPP
#define TRAINMANAGER_HPP

#include <vector>

#include "Pathfinder.hpp"
#include "../types/TrainLayer.hpp"
#include "../types/InstanceResult.hpp"

using namespace geode::prelude;

class TrainManager {
    static TrainManager* instance;

    std::vector<TrainLayer*> instances;
    long long timewarp;
    GJGameLevel* level;
    Pathfinder* pathfinder = nullptr;
    bool ready = false;
    std::vector<TrainLayer*> deadInstances;
    bool quitting = false;
    bool cbf = false;
    Mod* cbfMod = nullptr;

    TrainLayer* addInstance();

    TrainManager(
        const long long timewarp,
        GJGameLevel* level
    ) : timewarp(timewarp), level(level) {}

public:
    void assignTasks(const std::vector<NodeBranch> &instructions, const std::function<void(std::vector<InstanceResult>)> &callback);

    TrainLayer* getInstance(const PlayLayer* instance) const;
    [[nodiscard]] long long getTimewarp() const;
    void onDeath(TrainLayer* instance);
    [[nodiscard]] bool readyForNextAttempt() const;
    void onQuit();
    [[nodiscard]] bool isQuitting() const;

    static TrainManager* create(GJGameLevel* level);
    static TrainManager* get();
};

#endif