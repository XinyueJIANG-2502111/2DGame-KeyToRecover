#pragma once

#include "platform/dxlib/AudioManager.hpp"
#include "engine/components/BackgroundComponent.hpp"
#include "engine/Global.hpp"
#include "platform/input/InputMapping.hpp"
#include "game/PlayerProgress.hpp"
#include "platform/dxlib/ResourceManager.hpp"
#include "engine/scene/Scene.hpp"
#include "engine/scene/SceneManager.hpp"

class ResultScene : public Scene {
public:
    explicit ResultScene(const PlayerProgress& progress) : m_Progress(progress) {}

    void Initialize() override;
    void Update(double deltaTime) override;
    void Draw() override;
    void Finalize() override;

private:
    struct ActionIcon {
        InputAction action;
        const char* path;
        float offset;
        int handle = -1;
    };

    inline static constexpr const char* kBackgroundPath = "Resource/images/ClearScreen.png";
    inline static constexpr const char* kPlayerPath = "Resource/images/Player.png";

    const PlayerProgress m_Progress;
    Vector2f m_PlayerPosition{ 0.f, 1020.f };
    int m_PlayerHandle = -1;
    ActionIcon m_Icons[5] = {
        { InputAction::JUMP, "Resource/images/Space.png", 100.f },
        { InputAction::MOVE_LEFT, "Resource/images/プレイヤー「A」.png", 200.f },
        { InputAction::MOVE_RIGHT, "Resource/images/プレイヤー「D」.png", 300.f },
        { InputAction::ATTACK, "Resource/images/プレイヤー「L」.png", 400.f },
        { InputAction::PICKUP, "Resource/images/プレイヤー「J」.png", 500.f },
    };
};

inline void ResultScene::Initialize() {
    auto& resources = ResourceManager::GetInstance();
    const int bgHandle = resources.LoadGraphic(kBackgroundPath);
    int bgWidth = Config::SCREEN_WIDTH;
    int bgHeight = Config::SCREEN_HEIGHT;
    GraphicsManager::GetInstance().GetGraphSize(bgHandle, &bgWidth, &bgHeight);
    auto background = std::make_unique<GameObject>();
    background->SetZOrder(-1.f);
    background->AddComponent<BackgroundComponent>(bgHandle, 1.f);
    AddObject(std::move(background));
    SetWorldBounds(Vector2f(0.f, 0.f), Vector2f(bgWidth, bgHeight));

    m_Camera = std::make_unique<Camera>(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    m_Camera->SetPosition(m_Camera->WorldToScreen(Vector2f(0.f, 0.f)));
    m_Camera->SetBounds(bgWidth, bgHeight);
    m_Camera->SetDeadzone(600, 400);

    for (auto& icon : m_Icons) {
        if (m_Progress.HasCollected(icon.action)) {
            icon.handle = resources.LoadGraphic(icon.path);
        }
    }
    m_PlayerHandle = resources.LoadGraphic(kPlayerPath);
    AudioManager::GetInstance()->StopMusic();
}

inline void ResultScene::Update(double deltaTime) {
    Scene::Update(deltaTime);
    m_PlayerPosition.x += 100.f * static_cast<float>(deltaTime);
    if (m_PlayerPosition.x > Config::SCREEN_WIDTH + 100.f) {
        m_PlayerPosition.x = -100.f;
    }

    if (InputMapping::GetInstance().IsJustPressed(InputAction::CONFIRM)) {
        SceneManager::GetInstance().ChangeScene(SceneType::SCENETYPE_TITLE);
    }
}

inline void ResultScene::Draw() {
    Scene::Draw();
    const int y = static_cast<int>(m_PlayerPosition.y);
    for (const auto& icon : m_Icons) {
        if (icon.handle != -1) {
            DrawGraph(static_cast<int>(m_PlayerPosition.x - icon.offset), y, icon.handle, TRUE);
        }
    }
    DrawGraph(static_cast<int>(m_PlayerPosition.x), y, m_PlayerHandle, TRUE);

    SetFontSize(96);
    DrawFormatString(660, 540, GetColor(255, 255, 255), "input count: %d", m_Progress.inputCount);
    SetFontSize(16);
}

inline void ResultScene::Finalize() {
    auto& resources = ResourceManager::GetInstance();
    RemoveAllObjects();
    resources.ReleaseGraphic(kBackgroundPath);
    resources.ReleaseGraphic(kPlayerPath);
    for (auto& icon : m_Icons) {
        if (icon.handle != -1) {
            resources.ReleaseGraphic(icon.path);
            icon.handle = -1;
        }
    }
}
