#pragma once
#include "engine/scene/SceneManager.hpp"
#include "platform/dxlib/ResourceManager.hpp"
#include "engine/scene/Scene.hpp"


#include "engine/components/BackgroundComponent.hpp"
#include "game/effects/FloatingBlocks.hpp" // ★追加

#include "engine/scene/Camera.hpp"

#include "platform/dxlib/AudioManager.hpp"

#include "platform/input/InputMapping.hpp"
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <type_traits>



// タイトルシーン
class TitleScene : public Scene {
public:
    void Initialize() override;
    void Update(double deltaTime) override;
    void Finalize() override;

private:
    bool isStartingGame = false;
    float fadeAlpha = 0.0f;
    GameObject* fadeObject = nullptr;
   
};

void TitleScene::Initialize() {

    // ==========================================
    // 2. タイトルのUI画像（title.PNG）の生成 (Z: 0.0f)
    // ==========================================
    int bgHandle = ResourceManager::GetInstance().LoadGraphic("Resource/images/Title_background.png");
    int bgX, bgY;
    GraphicsManager::GetInstance().GetGraphSize(bgHandle, &bgX, &bgY);

    auto bgObject = std::make_unique<GameObject>();
    // ★重要: ブロック(-1.0f)より手前に描画するため、Z-Orderを 0.f に変更
    bgObject->SetZOrder(0.f);
    bgObject->AddComponent<BackgroundComponent>(bgHandle, 1.f);
    AddObject(std::move(bgObject));

    SetWorldBounds(Vector2f(0.f, 0.f), Vector2f(bgX, bgY));

    // ==========================================
    // 3. カメラの生成
    // ==========================================
    m_Camera = std::make_unique<Camera>(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    m_Camera->SetPosition(m_Camera->WorldToScreen(Vector2f(0.f, 0.f)));
    m_Camera->SetBounds(bgX, bgY);
    m_Camera->SetDeadzone(600, 400);

    // ==========================================
    // 1. プワプワ浮くブロック群の生成 (Z: -1.0f)
    // ==========================================
    auto floatingObj = std::make_unique<FloatingBlocksObject>();
    floatingObj->SetZOrder(2.f);
    AddObject(std::move(floatingObj));

    // ==========================================
    // 4. フェードアウト用の黒パネル (Z: 9999.0f)
    // ==========================================
    int blackHandle = ResourceManager::GetInstance().LoadGraphic("Resource/images/BlackPanel.png");
    auto blackObj = std::make_unique<GameObject>();

    blackObj->SetZOrder(9999.0f); // 最前面
    auto* bg = blackObj->AddComponent<BackgroundComponent>(blackHandle, 1.0f);

    fadeObject = blackObj.get();
    AddObject(std::move(blackObj));
    bg->SetAlpha(0);
}

void TitleScene::Update(double deltaTime) {
    Scene::Update(deltaTime);

  //  // 決定（シーン遷移）
  //  if (InputMapping::GetInstance().IsJustPressed(InputAction::CONFIRM)) {
  //      SceneManager::GetInstance().ChangeScene(SceneType::SCENETYPE_GAME_STAGE1);
		//AudioManager::GetInstance()->PlaySound("Resource/sounds/SE/決定.mp3");
  //  }
   
    if (!isStartingGame)
    {
        if (InputMapping::GetInstance().IsJustPressed(InputAction::CONFIRM))
        {
            isStartingGame = true;
        }
    }
    else
    {
        fadeAlpha += 0.03f;

        auto* bg =
            fadeObject->GetComponent<BackgroundComponent>();

        if (bg)
        {
            bg->SetAlpha(fadeAlpha);
        }

        if (fadeAlpha >= 1.0f)
        {
            SceneManager::GetInstance().ChangeScene(
                SceneType::SCENETYPE_GAME_STAGE1
            );
        }
    }
}

void TitleScene::Finalize() {
}

