//
// Created by akabane on 2014/04/23.
//

#include "GameScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

cocos2d::Scene* Game::createScene() {
    auto scene = Scene::create();
    auto layer = Game::create();
    scene->addChild(layer);

    return scene;
}

Game::Game()
: Layer()
, mpScoreLabel(nullptr)
, mScoreValue(0)
, mpTimeLabel(nullptr)
, mTimeValue(0)
{
}

bool Game::init() {
    if (!Layer::init()) {
        return false;
    }

    // 背景を表示
    Sprite* background = Sprite::create("background.png");
    background->setAnchorPoint(Point(0, 0));
    this->addChild(background);

    // スタートボタンを設置
    MenuItemFont* startButton = MenuItemFont::create("Start", [this](Ref* sender){
        static_cast<MenuItemFont*>(sender)->removeFromParent();
        this->start();
    });
    Menu* menu = Menu::create(startButton, nullptr);
    this->addChild(menu);

    // スコアラベルを設置
    mpScoreLabel = Label::createWithSystemFont("", "Arial-BoldMT", 24, Size(320, 50), TextHAlignment::LEFT, TextVAlignment::CENTER);
    mpScoreLabel->setAnchorPoint(Point(0.0f, 0.5f));
    mpScoreLabel->setPosition(10, 330);
    this->addChild(mpScoreLabel);

    // 時間ラベルを設置
    mpTimeLabel = Label::createWithSystemFont("", "Arial-BoldMT", 24, Size(320, 50), TextHAlignment::RIGHT, TextVAlignment::CENTER);
    mpTimeLabel->setAnchorPoint(Point(1.0f, 0.5f));
    mpTimeLabel->setPosition(320-10, 330);
    this->addChild(mpTimeLabel);

    // タッチのコールバックを設定
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        for (auto slime : mSlimes) {
            // タッチ位置との距離を算出
            const float distance = slime->getPosition().getDistance(touch->getLocation());
            // 一定範囲内なら触れたと判定
            if (distance <= 40.0f) {
                // 効果音再生
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("se_slime.wav");

                // スライムの種類別にスコア決定
                uint score = 0;
                switch (slime->getTag()) {
                    default: break;
                    case 1:
                        score = 100;
                        break;
                    case 2:
                        score = 300;
                        break;
                    case 3:
                        score = 1000;
                        break;
                }
                mScoreValue += score;

                this->showScore(slime->getPosition(), score);
                this->updateScoreLabel();
                this->setupSlime(slime);
                this->popSlime(slime);
                return false;
            }
        }
        return false;
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    // パラメータ初期化
    mTimeValue = 15; // 制限時間15秒
    this->updateScoreLabel();
    this->updateTimeLabel();

    return true;
}

void Game::update(float dt) {
    mTimeValue -= dt;
    mTimeValue = MAX(mTimeValue, 0);
    this->updateTimeLabel();

    if (mTimeValue <= 0) {
        this->finish();
    }
}

void Game::start() {
    auto setupGame = [this](){
        // ゲーム開始
        static const uint numSlimes = 3;
        for (uint i=0; i<numSlimes; ++i) {
            Sprite* slime = Sprite::create();
            this->setupSlime(slime);
            this->addChild(slime);
            this->popSlime(slime);

            // 保存しておく
            mSlimes.pushBack(slime);
        }

        this->scheduleUpdate();
    };

    // 開始テキスト表示
    Label* label = Label::createWithSystemFont("START!", "Arial-BoldMT", 40, Size(320, 50), TextHAlignment::CENTER, TextVAlignment::CENTER);
    this->addChild(label);

    // 右から現れて左に消える
    static const float labelY = 200;
    static const float marginX = 200;
    label->setPosition(320+marginX, labelY);
    label->runAction(
            Sequence::create(
                    MoveTo::create(0.2f, Point(160, labelY)),
                    DelayTime::create(1.0f),
                    MoveTo::create(0.2f, Point(-marginX, labelY)),
                    CallFunc::create(setupGame), // 開始処理を呼び出し
                    RemoveSelf::create(),
                    NULL
            )
    );
}

void Game::finish() {
    for (auto slime : mSlimes) {
        slime->stopAllActions();
        slime->removeFromParent();
    }
    mSlimes.clear();
    this->unscheduleUpdate();

    auto showResetButton = [this](){
        // リセットボタンを設置
        MenuItemFont* resetButton = MenuItemFont::create("Reset", [this](Ref* sender){
            static_cast<MenuItemFont*>(sender)->removeFromParent();
            // ゲームシーンを再読み込みして初期化する
            Scene* next = Game::createScene();
            Director::getInstance()->replaceScene(next);
        });
        Menu* menu = Menu::create(resetButton, nullptr);
        this->addChild(menu);
    };

    // 終了テキスト表示
    Label* label = Label::createWithSystemFont("FINISH!", "Arial-BoldMT", 40, Size(320, 50), TextHAlignment::CENTER, TextVAlignment::CENTER);
    this->addChild(label);

    // 右から現れて左に消える
    static const float labelY = 200;
    static const float marginX = 200;
    label->setPosition(320+marginX, labelY);
    label->runAction(
            Sequence::create(
                    MoveTo::create(0.2f, Point(160, labelY)),
                    DelayTime::create(1.0f),
                    MoveTo::create(0.2f, Point(-marginX, labelY)),
                    CallFunc::create(showResetButton),
                    RemoveSelf::create(),
                    NULL
            )
    );

}

void Game::setupSlime(cocos2d::Sprite* target) {
    target->stopAllActions();

    // スライムの種類をランダムに決定し、画像とアニメーションをセット
    const float random = CCRANDOM_0_1();
    uint slimeID = 0;
    if (random <= 0.3f) {
        slimeID = 1;
    }
    else if (random <= 0.6f) {
        slimeID = 2;
    }
    else {
        slimeID = 3;
    }

    target->setTexture(StringUtils::format("slime_%u_1.png", slimeID));
    target->setTag(slimeID); // タグをスライムの種類を表す数値として扱う

    // パラパラアニメで動かす
    Animation* animation = Animation::create();
    animation->addSpriteFrameWithFile(StringUtils::format("slime_%u_1.png", slimeID));
    animation->addSpriteFrameWithFile(StringUtils::format("slime_%u_2.png", slimeID));
    animation->setDelayPerUnit(0.5f); // 1コマあたりの秒数を指定
    target->runAction(
            RepeatForever::create(
                    Animate::create(animation)
            )
    );
}

void Game::popSlime(cocos2d::Sprite* target) {
    target->setVisible(true);

    static const float margin = 30.0f; // 画面端には移動しないようにする
    const float x = margin + CCRANDOM_0_1() * (320.0f - margin*2);
    const float y = margin + CCRANDOM_0_1() * 200.0f;
    target->setPosition(x, y);

    // 手前にいるほど優先的に描画されるようにする
    target->setLocalZOrder(1024 - (int)y);

    // 一定時間後に移動
    static const int rePopTag = 100;
    target->stopActionByTag(rePopTag);

    float delay = 0;
    switch (target->getTag()) {
        default:
            break;
        case 1:
            delay = 2.0f;
            break;
        case 2:
            delay = 1.0f;
            break;
        case 3:
            delay = 0.5f;
            break;
    }

    Action* rePop = Sequence::create(
            DelayTime::create(delay),
            CallFunc::create([this, target](){
                this->setupSlime(target);
                this->popSlime(target);
            }),
            nullptr
    );
    rePop->setTag(rePopTag);
    target->runAction(rePop);
}

void Game::showScore(const cocos2d::Point &point, uint score) {
    Label* label = Label::createWithSystemFont(StringUtils::format("%u", score), "Arial-BoldMT", 24, Size(160, 50), TextHAlignment::CENTER, TextVAlignment::CENTER);
    label->setPosition(point);
    this->addChild(label, 1024);

    // 拡大しながら現れて、一定時間後に消える
    label->setScale(0);
    label->runAction(
            Sequence::create(
                    ScaleTo::create(0.1f, 1.0f),
                    DelayTime::create(0.5f),
                    RemoveSelf::create(),
                    nullptr
            )
    );
}

void Game::updateScoreLabel() {
    mpScoreLabel->setString(StringUtils::format("%upts", mScoreValue));
}

void Game::updateTimeLabel() {
    mpTimeLabel->setString(StringUtils::format("TIME: %.1f", mTimeValue));
}
