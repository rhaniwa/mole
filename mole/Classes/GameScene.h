//
// Created by akabane on 2014/04/23.
//


#ifndef __GameScene_H__
#define __GameScene_H__

#include "cocos2d.h"

class Game : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene();

public:
    Game();
    virtual ~Game() {};

    virtual bool init();
    CREATE_FUNC(Game);

    void update(float dt);

private:
    void start();
    void finish();

    void setupSlime(cocos2d::Sprite* target);
    void popSlime(cocos2d::Sprite* target);
    void showScore(const cocos2d::Point &point, uint score);
    void updateScoreLabel();
    void updateTimeLabel();

private:
    cocos2d::Vector<cocos2d::Sprite*>   mSlimes;

    cocos2d::Label* mpScoreLabel;
    uint            mScoreValue;

    cocos2d::Label* mpTimeLabel;
    float           mTimeValue;

};

#endif //__GameScene_H__
