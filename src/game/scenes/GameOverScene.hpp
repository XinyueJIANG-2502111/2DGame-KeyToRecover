#pragma once
# include "platform/dxlib/AudioManager.hpp"
# include "platform/input/InputMapping.hpp"
# include "platform/dxlib/ResourceManager.hpp"
# include "engine/scene/SceneManager.hpp"
# include "engine/scene/Scene.hpp"
# include "engine/components/BackgroundComponent.hpp"
#include "game/effects/GlitchBackgroundObject.hpp"
# include "engine/scene/Camera.hpp"

class GameOverScene : public Scene
{
public:
    void Initialize() override;
    void Update(double deltaTime) override;
    void Finalize() override;
};

void GameOverScene::Initialize() {
   
    int bgHandle = ResourceManager::GetInstance().LoadGraphic("Resource/images/GameOverScreen.png");
    int bgX, bgY;
    GraphicsManager::GetInstance().GetGraphSize(bgHandle, &bgX, &bgY);

    auto bgObject = std::make_unique<GlitchBackgroundObject>(bgHandle);

    AddObject(std::move(bgObject));
    SetWorldBounds(Vector2f(0.f, 0.f), Vector2f(bgX, bgY));


    // create camera
    m_Camera = std::make_unique<Camera>(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    m_Camera->SetPosition(m_Camera->WorldToScreen(Vector2f(0.f, 0.f)));
    m_Camera->SetBounds(bgX, bgY);
    m_Camera->SetDeadzone(600, 400);

    AudioManager::GetInstance()->StopMusic();
    AudioManager::GetInstance()->PlayMusic("Resource/sounds/BGM/GameOverScene.mp3", true);
}

void GameOverScene::Update(double deltaTime) {
    Scene::Update(deltaTime);
    if (InputMapping::GetInstance().IsJustPressed(InputAction::CONFIRM)) {
        SceneManager::GetInstance().ChangeScene(SceneType::SCENETYPE_TITLE);
    }
}

void GameOverScene::Finalize() {
}