#include "Game.hpp"

#include "DxLib.h"
#include "game/scenes/GameOverScene.hpp"
#include "engine/Global.hpp"
#include "platform/dxlib/GraphicsManager.hpp"
#include "platform/input/InputManager.hpp"
#include "platform/input/InputMapping.hpp"
#include "platform/dxlib/ResourceManager.hpp"
#include "game/scenes/ResultScene.hpp"
#include "engine/scene/SceneManager.hpp"
#include "game/scenes/Stage1.hpp"
#include "game/scenes/TitleScene.hpp"

namespace {
    constexpr double TARGET_FPS = 60.0;
    constexpr double FIXED_TIMESTEP = 1.0 / TARGET_FPS;
}

Game::Game() : mb_IsRunning(false), mb_initSuccess(false), md_Accumulator(0.0)
{
    //--------------------------------------------------------------//
    //--------------------------------------------------------------//
    // DxLib setup                                                  //
    ChangeWindowMode(TRUE);                                         //
    SetGraphMode(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, 32);  //
    SetMainWindowText("2DTest");                                    //
                                                                    //
    if (DxLib_Init() == -1)                                         //
    {                                                               //
        return;                                                     //
    }                                                               //
                                                                    //
    SetDrawScreen(DX_SCREEN_BACK);                                  //
    mb_initSuccess = true;                                          //
    mb_IsRunning = true;                                            //
                                                                    //
    m_LastFrameTime = std::chrono::steady_clock::now();             //
    //--------------------------------------------------------------//
    //--------------------------------------------------------------//


    GraphicsManager::GetInstance().Initialize(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    
    // -----------------------------------------------------------
    // TODO : register scenes（ここで新しく作ったシーンを登録する）
    // test scene

    SceneManager::GetInstance().RegisterScene(SceneType::SCENETYPE_TITLE,
        []() { return std::make_unique<TitleScene>(); });
    SceneManager::GetInstance().RegisterScene(SceneType::SCENETYPE_GAME_STAGE1,
        [this]() { return std::make_unique<Stage1>(m_Progress); });

    SceneManager::GetInstance().RegisterScene(SceneType::SCENETYPE_GAMEOVER,
        []() { return std::make_unique<GameOverScene>(); });
    SceneManager::GetInstance().RegisterScene(SceneType::SCENETYPE_RESULT,
        [this]() { return std::make_unique<ResultScene>(m_Progress); });
    // -----------------------------------------------------------

    // set
    SceneManager::GetInstance().ChangeScene(SceneType::SCENETYPE_TITLE);
}

Game::~Game() {
    SceneManager::GetInstance().Finalize();
    DxLib_End();
}

// game loop
void Game::Run()
{
    while (mb_IsRunning && ProcessMessage() == 0) {
        auto currentTimePoint = std::chrono::steady_clock::now();
        double deltaTime = std::chrono::duration<double>(currentTimePoint - m_LastFrameTime).count();
        m_LastFrameTime = currentTimePoint;

        md_Accumulator += deltaTime;

        while (md_Accumulator >= FIXED_TIMESTEP) {
            UpdateLogic(FIXED_TIMESTEP);
            md_Accumulator -= FIXED_TIMESTEP;
        }

        double interpolationAlpha = md_Accumulator / FIXED_TIMESTEP;

        UpdateRender(interpolationAlpha);
        Draw();
        ScreenFlip();
    }
}

void Game::UpdateLogic(double deltaTime)
{
    InputManager::GetInstance().Update();
    InputMapping::GetInstance().Update();

    ResourceManager::GetInstance().Update();

    SceneManager::GetInstance().Update(deltaTime);

    if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_ESCAPE))
    {
        mb_IsRunning = false;
    }
}

void Game::UpdateRender(double interpolationAlpha) {
}

void Game::Draw()
{
    ClearDrawScreen();
    SceneManager::GetInstance().Draw();
}