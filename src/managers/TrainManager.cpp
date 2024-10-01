#include "TrainManager.hpp"

#include "../layers/Grid.hpp"

using namespace geode::prelude;

TrainManager* TrainManager::instance = nullptr;

PlayLayer *TrainManager::addInstance() {
    const auto layer = new PlayLayer();
    instances.emplace_back(layer);
    layer->setObjType(CCObjectType::PlayLayer);
    layer->init(this->level, false, false);
    layer->m_isPracticeMode = true;
    layer->m_gameState.m_timeWarp = timewarp;
    layer->autorelease();


    return layer;
}

void TrainManager::assignTask(const std::vector<std::vector<bool>>& instructions, std::function<void(std::vector<std::pair<bool, size_t>>)> callback) {
    std::vector<std::pair<bool, size_t>> results;
    for (const auto& instruction : instructions) {
        bool success = true; // Placeholder
        size_t failIndex = 0; // Placeholder
        results.emplace_back(success, failIndex);
    }
    callback(results);
}

bool TrainManager::checkInstance(const PlayLayer* instance) const {
    for (auto& i : instances) {
        if (i == instance) {
            return true;
        }
    }
    return false;
}

long long TrainManager::getTimewarp() const {
    return timewarp;
}


TrainManager *TrainManager::create(GJGameLevel* level) {
    instance = new TrainManager(Mod::get()->getSettingValue<int64_t>("timewarp"), level);

    const auto n = Mod::get()->getSettingValue<int64_t>("n");

    instance->pathfinder = new Pathfinder(instance, static_cast<size_t>(n));

    long long instances = 0;
    int cols;
    if (n == 1) {
        instances = 2;
        cols = 2;
    }
    else {
        instances = n*n;
        cols = n;
    }

    auto scene = CCScene::create();
    auto array = CCArray::create();

    for (int i = 0; i < instances; i++) {
        auto layer = instance->addInstance();
        array->addObject(layer);
        log::info("new id {}", layer->m_player1->m_uID);
    }

    const auto grid = Grid::create(n, cols, array);
    scene->addChild(grid);

    CCDirector::get()->pushScene(scene);

    return instance;
}

TrainManager *TrainManager::get() {
    return instance;
}