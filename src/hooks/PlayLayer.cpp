#include <Geode/modify/PlayLayer.hpp>

#include "../managers/TrainManager.hpp"

using namespace geode::prelude;

constexpr std::array removal = {
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    32,
    33,
    55,
    56,
    57,
    58,
    59,
    104,
    105,
    221,
    717,
    718,
    743,
    744,
    899,
    900,
    915,
    1006,
    1007,
    1520,
    1586,
    1615,
    1818,
    1819,
    1915,
    1934,
    2016,
    2065,
    2903,
    2904,
    2905,
    2907,
    2909,
    2910,
    2911,
    2912,
    2913,
    2914,
    2915,
    2916,
    2917,
    2919,
    2920,
    2921,
    2922,
    2923,
    2924,
    2999,
    3009,
    3010,
    3014,
    3015,
    3017,
    3018,
    3019,
    3020,
    3021,
    3023,
    3029,
    3030,
    3031,
    3602,
    3603,
    3605,
    3606,
    3608,
    3612,
    3642
};

class $modify(APlayLayer, PlayLayer) {
    struct Fields {
        TrainLayer* trainLayer = nullptr;
        TrainManager* trainManager = nullptr;
    };

    bool checkTrainLayer() {
        if (m_fields->trainLayer) return true;
        if (!m_fields->trainManager) {
            m_fields->trainManager = TrainManager::get();
        }
        if (!m_fields->trainManager) return false;
        m_fields->trainLayer = m_fields->trainManager->getInstance(this);
        return m_fields->trainLayer != nullptr;
    }

    void addObject(GameObject* obj) {
        if (!checkTrainLayer()) PlayLayer::addObject(obj);
        PlayLayer::addObject(obj);
        obj->m_activeMainColorID = -1;
        obj->m_activeDetailColorID = -1;
        obj->m_detailUsesHSV = false;
        obj->m_baseUsesHSV = false;
        obj->m_hasNoGlow = true;
        obj->m_isHide = false;
        obj->setOpacity(255);
        obj->setVisible(true);
        return;
        if (obj->m_hasGroupParentsString || (obj->m_groups && !obj->m_groups->empty()) || obj->m_hasAreaParent || obj->m_hasGroupParent || obj->m_objectID == 44 || obj->m_enterChannel) {
            PlayLayer::addObject(obj);
            return;
        }
        if (obj->m_isDecoration || obj->m_objectType == GameObjectType::Decoration || std::ranges::find(removal, obj->m_objectID) != std::ranges::end(removal)) {
            return;
        }
    }

    void resetLevel() {
        if (!checkTrainLayer()) PlayLayer::resetLevel();

        if (m_fields->trainManager->readyForNextAttempt()) {
            if (const auto old_playlayer = GameManager::get()->m_playLayer) {
                GameManager::get()->m_playLayer = this;
                PlayLayer::resetLevel();
                GameManager::get()->m_playLayer = old_playlayer;
            } else {
                PlayLayer::resetLevel();
            }
            return;
        }

        m_fields->trainManager->onDeath(m_fields->trainLayer);

        m_gameState.m_timeWarp = m_fields->trainManager->getTimewarp();
    }

    void destroyPlayer(PlayerObject* player, GameObject* gameObject) {
        PlayLayer::destroyPlayer(player, gameObject);
        if (!checkTrainLayer()) return;

        if (const auto respawn = this->getActionByTag(0x10)) {
            this->stopAction(respawn);
            this->delayedResetLevel();
        }
    }
};