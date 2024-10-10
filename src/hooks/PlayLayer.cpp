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
    1612,
    1613,
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

        bool checkedTrainManager = false;
    };

    bool checkTrainLayer() {
        const auto fields = m_fields.self();

        if (fields->trainLayer) return true;
        if (!fields->trainManager && !fields->checkedTrainManager) {
            fields->trainManager = TrainManager::get();
            fields->checkedTrainManager = true;
        }
        if (!fields->trainManager) return false;
        fields->trainLayer = fields->trainManager->getInstance(this);
        return fields->trainLayer;
    }

    // Thanks iAndy_HD3 for this snippet
    // https://discord.com/channels/911701438269386882/911702535373475870/1291313907222450176 (Geode SDK Discord)
    static gd::vector<short> getGroupIDs(const GameObject* obj) {
        gd::vector<short> res;

        if (obj->m_groups && obj->m_groups->at(0))
            for (auto i = 0; i < obj->m_groupCount; i++)
                res.push_back(obj->m_groups->at(i));
        return res;
    }

    void addObject(GameObject* obj) {
        if (!checkTrainLayer()) return PlayLayer::addObject(obj);

        // Thanks TheSillyDoggo for helping me get this to work
        // https://discord.com/channels/911701438269386882/911702535373475870/1291278375553466420 (Geode SDK Discord)

        bool cancelAddition = false;

        if (std::ranges::find(removal, obj->m_objectID) != std::ranges::end(removal)) cancelAddition = true;
        if (obj->m_isNoTouch || obj->m_objectType == GameObjectType::Decoration) cancelAddition = true;
        if (obj->m_groups && m_levelSettings->m_spawnGroup != 0) if (const auto groups = getGroupIDs(obj); std::ranges::find(groups, m_levelSettings->m_spawnGroup) != groups.end()) cancelAddition = false;
        if (obj->m_hasGroupParent || obj->m_hasAreaParent || obj->m_objectID == 749 || obj->m_objectID == 44) cancelAddition = false;

        if (!cancelAddition) {
            PlayLayer::addObject(obj);
            obj->m_activeMainColorID = -1;
            obj->m_activeDetailColorID = -1;
            obj->m_detailUsesHSV = false;
            obj->m_baseUsesHSV = false;
            obj->m_hasNoGlow = true;
            obj->m_isHide = false;
            obj->setOpacity(255);
            obj->setVisible(true);
        }
    }

    void resetLevel() {
        if (!checkTrainLayer()) return PlayLayer::resetLevel();

        const auto fields = m_fields.self();

        if (fields->trainManager->readyForNextAttempt()) {
            if (const auto old_playlayer = GameManager::get()->m_playLayer) {
                GameManager::get()->m_playLayer = this;
                PlayLayer::resetLevel();
                GameManager::get()->m_playLayer = old_playlayer;
            } else {
                PlayLayer::resetLevel();
            }
            this->m_gameState.m_timeWarp = fields->trainManager->getTimewarp();
            return;
        }
        fields->trainManager->onDeath(fields->trainLayer);
    }

    void destroyPlayer(PlayerObject* player, GameObject* gameObject) {
        PlayLayer::destroyPlayer(player, gameObject);
        if (!checkTrainLayer()) return;

        if (const auto respawn = this->getActionByTag(0x10)) {
            this->stopAction(respawn);
            this->delayedResetLevel();
        }
    }

    void storeCheckpoint(CheckpointObject* checkpointObject) {
        if (!checkTrainLayer()) return PlayLayer::storeCheckpoint(checkpointObject);

        m_checkpointArray->addObject(checkpointObject);
        PlayLayer::addToSection(checkpointObject->m_physicalCheckpointObject);
    }

    void onQuit() {
        if (!checkTrainLayer()) return PlayLayer::onQuit();

        const auto fields = m_fields.self();

        if (fields->trainManager->isQuitting()) {
            PlayLayer::onQuit();
            log::info("quitting");
            m_uiLayer->removeFromParentAndCleanup(true);
            removeFromParentAndCleanup(true);
            return;
        }

        fields->trainManager->onQuit();
    }
};