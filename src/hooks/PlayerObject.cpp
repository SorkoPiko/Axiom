#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(APlayerObject, PlayerObject) {
    void collidedWithObjectInternal(const float p0, GameObject* p1, CCRect p2, const bool p3) {
        PlayerObject::collidedWithObjectInternal(p0, p1, p2, p3);
    }
};