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
    void addObject(GameObject* obj) {
        const auto trainManager = TrainManager::get();
        if (trainManager && trainManager->checkInstance(this)) {
            if (obj->m_objectType == GameObjectType::Decoration && obj->m_objectID != 44 || obj->m_isNoTouch || std::find(removal.begin(), removal.end(), obj->m_objectID) == removal.end()) {
                // if (!obj->m_groups->empty() || obj->m_hasAreaParent || obj->m_hasGroupParent) {
                if (!obj->m_groups->empty() || obj->m_hasAreaParent || obj->m_hasGroupParent) {
                    return PlayLayer::addObject(obj);
                }
                return;
            }
        }
        PlayLayer::addObject(obj);
    }

    void resetLevel() {
        const auto trainManager = TrainManager::get();
        if (trainManager && trainManager->checkInstance(this)) {
            const auto old_playlayer = GameManager::get()->m_playLayer;
            GameManager::get()->m_playLayer = this;
            PlayLayer::resetLevel();
            GameManager::get()->m_playLayer = old_playlayer;

            m_gameState.m_timeWarp = trainManager->getTimewarp();
            return;
        }
        PlayLayer::resetLevel();
    }
};