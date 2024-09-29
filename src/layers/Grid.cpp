#include "Grid.hpp"

using namespace geode::prelude;

Grid *Grid::create(const int rows, const int cols, CCArray *items)  {
    auto ret = new Grid();
    if (ret->init(rows, cols, items)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool Grid::init(const int rows, const int cols, CCArray *items)  {
    if (!CCLayer::init()) return false;

    const auto winSize = CCDirector::sharedDirector()->getWinSize();

    const auto first = dynamic_cast<CCNode*>(items->objectAtIndex(0));
    float itemWidth = first->getContentSize().width;
    float itemHeight = first->getContentSize().height;
    float gapX = itemWidth * 0.1f;
    float gapY = itemHeight * 0.1f;

    float totalWidth = cols * (itemWidth + gapX) - gapX;
    float totalHeight = rows * (itemHeight + gapY) - gapY;

    const float scaleX = (winSize.width * 0.9f) / totalWidth;
    const float scaleY = (winSize.height * 0.9f) / totalHeight;
    const float scale = fmin(scaleX, scaleY);

    itemWidth *= scale;
    itemHeight *= scale;
    gapX *= scale;
    gapY *= scale;
    totalWidth *= scale;
    totalHeight *= scale;

    CCNode* container = CCNode::create();
    container->setAnchorPoint(ccp(0.5f, 0.5f));
    this->addChild(container);

    for (int i = 0; i < items->count(); i++) {
        const auto item = dynamic_cast<CCNode*>(items->objectAtIndex(i));

        const float x = i % cols * (itemWidth + gapX) - totalWidth / 2 + itemWidth / 2;
        const float y = totalHeight / 2 - (i / cols * (itemHeight + gapY) + itemHeight / 2);

        item->setPosition(ccp(x, y));
        item->setScale(scale);
        container->addChild(item);
    }

    container->setPosition(0,0);

    return true;
}
