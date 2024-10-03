#include "TrainManager.hpp"

#include "../layers/Grid.hpp"

using namespace geode::prelude;

TrainManager* TrainManager::instance = nullptr;

TrainLayer* TrainManager::addInstance() {
    const auto layer = new TrainLayer(new PlayLayer());
    instances.emplace_back(layer);
    layer->playLayer->setObjType(CCObjectType::PlayLayer);
    layer->playLayer->init(this->level, false, false);
    layer->playLayer->m_isPracticeMode = true;
    layer->playLayer->m_gameState.m_timeWarp = timewarp;
    layer->playLayer->autorelease();

    return layer;
}

void TrainManager::assignTasks(const std::vector<std::vector<bool>>& instructions, const std::function<void(std::vector<std::pair<bool, size_t>>)>& callback) {
    std::vector<std::pair<bool, size_t>> results;
    constexpr std::array predefinedInstructions = {false, true, true, false, true};
    for (auto instruction : instructions) {
        auto success = true;
        size_t failIndex = -1;
        for (size_t i = 0; i < instruction.size(); i++) {
            if (instruction[i] != predefinedInstructions[i]) {
                if (predefinedInstructions[i] == predefinedInstructions.back()) success = true;
                else success = false;
                failIndex = i;
                break;
            }
        }
        results.emplace_back(success, failIndex);
    }
    callback(results);
}

TrainLayer* TrainManager::getInstance(const PlayLayer* instance) const {
    if (instance == nullptr) {
        log::error("supplied instance is null");
    }
    for (auto& i : instances) {
        if (i->playLayer == nullptr) {
            log::error("playLayer is null");
        }
        if (i->playLayer == instance) {
            return i;
        }
    }
    return nullptr;
}

long long TrainManager::getTimewarp() const {
    return timewarp;
}

std::pair<int, int> closestFactors(int number) {
    const auto sqrtNumber = static_cast<int>(std::sqrt(number));

    for (int i = sqrtNumber; i > 0; --i) {
        if (number % i == 0) {
            return {i, number / i};
        }
    }
    return {1, number};
}

void TrainManager::onDeath(TrainLayer* instance) {
    deadInstances.emplace_back(instance);
    if (readyForNextAttempt()) {
        for (auto& i : deadInstances) {
            i->playLayer->resetLevel();
        }
        deadInstances.clear();
    }
}

bool TrainManager::readyForNextAttempt() const {
    return deadInstances.size() == instances.size();
}

void TrainManager::onQuit() {
    quitting = true;
    for (auto& i : instances) {
        i->playLayer->onQuit();
    }

    //save and stuff ig

    delete pathfinder;
}

bool TrainManager::isQuitting() const {
    return quitting;
}

TrainManager *TrainManager::create(GJGameLevel* level) {
    instance = new TrainManager(Mod::get()->getSettingValue<int64_t>("timewarp"), level);

    const auto n = Mod::get()->getSettingValue<int64_t>("n");

    instance->pathfinder = new Pathfinder(instance, static_cast<size_t>(n));
    instance->pathfinder->test();

    const auto instances = 1 << n;
    const auto [rows, cols] = closestFactors(instances);

    const auto scene = CCScene::create();
    const auto array = CCArray::create();

    for (int i = 0; i < instances; i++) {
        const auto layer = instance->addInstance();
        array->addObject(layer->playLayer);
    }

    const auto grid = Grid::create(rows, cols, array);
    scene->addChild(grid);

    CCDirector::get()->pushScene(scene);

    return instance;
}

TrainManager *TrainManager::get() {
    return instance;
}