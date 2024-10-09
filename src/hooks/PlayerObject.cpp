#include <Geode/modify/PlayerObject.hpp>

#include "../managers/TrainManager.hpp"

using namespace geode::prelude;

class $modify(APlayerObject, PlayerObject) {
    struct Fields {
        PlayLayer* playLayer = nullptr;
        TrainLayer* trainLayer = nullptr;
        TrainManager* trainManager = nullptr;
        bool isLevelEditor = false;
    };

    bool checkTrainLayer() {
        const auto fields = m_fields.self();

        if (fields->isLevelEditor) return false;
        fields->isLevelEditor = LevelEditorLayer::get();
        if (fields->isLevelEditor) return false;
        if (!fields->playLayer) {
            fields->playLayer = static_cast<PlayLayer*>(m_gameLayer);
        }
        if (fields->trainLayer) return true;
        if (!fields->trainManager) {
            fields->trainManager = TrainManager::get();
        }
        if (!fields->trainManager) return false;
        fields->trainLayer = fields->trainManager->getInstance(fields->playLayer);
        return fields->trainLayer;
    }
};