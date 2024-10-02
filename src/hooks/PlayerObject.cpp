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
        if (m_fields->isLevelEditor) return false;
        m_fields->isLevelEditor = LevelEditorLayer::get() != nullptr;
        if (m_fields->isLevelEditor) return false;
        if (!m_fields->playLayer) {
            m_fields->playLayer = static_cast<PlayLayer*>(m_gameLayer);
        }
        if (m_fields->trainLayer) return true;
        if (!m_fields->trainManager) {
            m_fields->trainManager = TrainManager::get();
        }
        if (!m_fields->trainManager) return false;
        m_fields->trainLayer = m_fields->trainManager->getInstance(m_fields->playLayer);
        return m_fields->trainLayer != nullptr;
    }

    void collidedWithObjectInternal(const float p0, GameObject* p1, CCRect p2, const bool p3) {
        if (checkTrainLayer()) {
            const auto old_playlayer = GameManager::get()->m_playLayer;
            GameManager::get()->m_playLayer = m_fields->playLayer;
            PlayerObject::collidedWithObjectInternal(p0, p1, p2, p3);
            GameManager::get()->m_playLayer = old_playlayer;
            return;
        }
        PlayerObject::collidedWithObjectInternal(p0, p1, p2, p3);
    }

    void collidedWithSlopeInternal(const float dt, GameObject *object, const bool forced) {
        if (checkTrainLayer()) {
            const auto old_playlayer = GameManager::get()->m_playLayer;
            GameManager::get()->m_playLayer = m_fields->playLayer;
            PlayerObject::collidedWithSlopeInternal(dt, object, forced);
            GameManager::get()->m_playLayer = old_playlayer;
            return;
        }
        PlayerObject::collidedWithSlopeInternal(dt, object, forced);
    }
};