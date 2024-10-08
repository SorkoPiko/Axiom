#include <Geode/modify/GJBaseGameLayer.hpp>

#include "../managers/TrainManager.hpp"

using namespace geode::prelude;

class $modify(AGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        TrainLayer* trainLayer = nullptr;
        TrainManager* trainManager = nullptr;
        bool levelEditorLayer = false;

        bool checkedTrainManager = false;
        bool checkedLevelEditor = false;
    };

    bool checkTrainLayer() {
        const auto fields = m_fields.self();

        if (fields->trainLayer) return true;

        if (fields->levelEditorLayer) return false;
        if (!fields->checkedLevelEditor) {
            fields->levelEditorLayer = LevelEditorLayer::get();
            fields->checkedLevelEditor = true;
        }
        if (fields->levelEditorLayer) return false;

        if (!fields->trainManager && !fields->checkedTrainManager) {
            fields->trainManager = TrainManager::get();
        }
        if (!fields->trainManager) return false;

        fields->trainLayer = fields->trainManager->getInstance(reinterpret_cast<PlayLayer*>(this));
        return fields->trainLayer;
    }

    void update(const float dt) {
        if (LevelEditorLayer::get()) return GJBaseGameLayer::update(dt);
        GameManager::get()->m_playLayer = reinterpret_cast<PlayLayer*>(this);
        GameManager::get()->m_gameLayer = this;
        GJBaseGameLayer::update(dt);
    }

    void processCommands(const float dt) {
        if (!checkTrainLayer()) return GJBaseGameLayer::processCommands(dt);

        const auto fields = m_fields.self();

        if (m_gameState.m_currentProgress == 0) fields->trainLayer->lastAction = false;

        bool action = true;
        if (!fields->trainLayer->instructions.empty()) {
            action = fields->trainLayer->instructions.at(m_gameState.m_currentProgress)->input;
        }

        if (action != fields->trainLayer->lastAction || true) {
            handleButton(action, 1, true);
        }

        fields->trainLayer->lastAction = action;

        GJBaseGameLayer::processCommands(dt);
    }
};