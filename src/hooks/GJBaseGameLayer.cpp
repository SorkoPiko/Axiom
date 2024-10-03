#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(AGJBaseGameLayer, GJBaseGameLayer) {
    void update(float dt) {
        if (LevelEditorLayer::get()) return GJBaseGameLayer::update(dt);
        GameManager::get()->m_playLayer = reinterpret_cast<PlayLayer*>(this);
        GameManager::get()->m_gameLayer = this;
        GJBaseGameLayer::update(dt);
    }
};