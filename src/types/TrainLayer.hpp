#ifndef TRAINLAYER_HPP
#define TRAINLAYER_HPP

struct TrainLayer {
    PlayLayer* playLayer;
    bool isDead = false;
    bool lastAction = false;
    NodeBranch instructions = {};

    explicit TrainLayer(PlayLayer* playLayer) : playLayer(playLayer) {}
};

#endif
