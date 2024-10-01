#include <Geode/modify/EditLevelLayer.hpp>

#include "../managers/TrainManager.hpp"

using namespace geode::prelude;

class $modify(AEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level)) return false;

        const auto menu = this->getChildByID("level-actions-menu");
        if (!menu) return false;

        const auto axiomButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_ncsLibraryBtn_001.png"),
            this,
            menu_selector(AEditLevelLayer::startAxiom)
        );

        menu->addChild(axiomButton);
        menu->updateLayout();

        return true;
    }

    void startAxiom(CCObject*) {
        TrainManager::create(m_level);
    }
};