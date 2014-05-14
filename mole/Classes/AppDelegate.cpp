#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }

    glview->setDesignResolutionSize(320, 480, ResolutionPolicy::SHOW_ALL);


    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = Game::createScene();

    // run
    director->runWithScene(scene);

    // 乱数初期化
    srand((unsigned)time(nullptr));

    // 効果音先読み
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("se_slime.wav");

    auto size = Director::getInstance()->getOpenGLView()->getFrameSize();
    CCLOG("%f, %f", size.width, size.height);

    // 値の保存
    UserDefault::getInstance()->setIntegerForKey("score", 1000);
    UserDefault::getInstance()->setDataForKey(<#(const char*)pKey#>, <#(const Data&)value#>)
    UserDefault::getInstance()->flush();

    // 値の読み込み
    const int score = UserDefault::getInstance()->getIntegerForKey("score");

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
