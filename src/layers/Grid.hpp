#ifndef GRID_HPP
#define GRID_HPP

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Grid final : public CCLayer {
public:
    static Grid* create(int rows, int cols, CCArray* items);

    bool init(int rows, int cols, CCArray* items);
};

#endif
