// Taken directly from Eclipse Menu, all credit goes to the original author
// https://github.com/EclipseMenu/EclipseMenu/blob/3e48c28951e7fae567e418e242b3b67df8869f04/src/hacks/Level/PracticeFix.cpp
#include <Geode/modify/CheckpointObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../managers/TrainManager.hpp"
#include "../types/FixPlayerCheckpoint.hpp"

using namespace geode::prelude;


class $modify(FixPlayLayer, PlayLayer) {
    struct Fields {
        std::unordered_map<CheckpointObject*, CheckpointData> m_checkpoints;
    };

    void onQuit() {
        m_fields->m_checkpoints.clear();
        PlayLayer::onQuit();
    }

    void resetLevel() {
        if (m_checkpointArray->count() <= 0)
            m_fields->m_checkpoints.clear();

        PlayLayer::resetLevel();
    }

    void loadFromCheckpoint(CheckpointObject* checkpoint) {
        auto* playLayer = static_cast<FixPlayLayer*>(get());

        if (const auto trainManager = TrainManager::get(); trainManager && trainManager->getInstance(this) && playLayer->m_fields->m_checkpoints.contains(checkpoint)) {
            PlayLayer::loadFromCheckpoint(checkpoint);

            CheckpointData& data = playLayer->m_fields->m_checkpoints[checkpoint];
            data.apply(playLayer->m_player1, playLayer->m_gameState.m_isDualMode ? playLayer->m_player2 : nullptr);

            return;
        }

        PlayLayer::loadFromCheckpoint(checkpoint);
    }

};

class $modify(ACheckpointObject, CheckpointObject) {
#ifdef GEODE_IS_ANDROID
    static CheckpointObject* create() { // this is so dumb
        auto result = CheckpointObject::create();
#else
    bool init() override {
        auto result = CheckpointObject::init();
#endif

        if (!TrainManager::get()) return result;

        auto* playLayer = static_cast<FixPlayLayer*>(FixPlayLayer::get());

        if (playLayer->m_gameState.m_currentProgress > 0) {
            CheckpointData data(playLayer->m_player1, playLayer->m_gameState.m_isDualMode ? playLayer->m_player2 : nullptr);
#ifdef GEODE_IS_ANDROID
            playLayer->m_fields->m_checkpoints[result] = data;
#else
            playLayer->m_fields->m_checkpoints[this] = data;
#endif
        }

        return result;
    }
};