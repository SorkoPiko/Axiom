#include <Geode/modify/GJBaseGameLayer.hpp>

#include "../managers/TrainManager.hpp"

using namespace geode::prelude;

class $modify(AGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        TrainLayer* trainLayer = nullptr;
        TrainManager* trainManager = nullptr;
        bool levelEditorLayer = false;
        PlayLayer* playLayer = nullptr;

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

        fields->playLayer = reinterpret_cast<PlayLayer*>(this);
        fields->trainLayer = fields->trainManager->getInstance(fields->playLayer);
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

        const auto tick = m_gameState.m_currentProgress;

        if (tick == 0) fields->trainLayer->lastAction = false;

        bool action = true;
        if (!fields->trainLayer->instructions.empty()) {
            action = fields->trainLayer->instructions.at(tick)->input;
        }

        if (action != fields->trainLayer->lastAction) {
            if (action)
                fields->playLayer->m_player1->pushButton(PlayerButton::Jump);
            else
                fields->playLayer->m_player1->releaseButton(PlayerButton::Jump);
            fields->trainLayer->lastAction = action;
        }

        GJBaseGameLayer::processCommands(dt);
    }
};