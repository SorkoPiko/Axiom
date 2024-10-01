#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(AGJBaseGameLayer, GJBaseGameLayer) {
    void processCommands(float dt) {
        GJBaseGameLayer::processCommands(dt);
    }
};