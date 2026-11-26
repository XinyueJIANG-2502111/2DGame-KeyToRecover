#pragma once
#include "engine/objects/GameObject.hpp"
#include "engine/Global.hpp"
#include "platform/input/InputManager.hpp"
#include "platform/dxlib/ResourceManager.hpp"
#include "engine/components/SpriteComponent.hpp"

#include <optional>

class Menu : public GameObject
{
public:
    enum class Option {
        ReturnToTitle,
        Restart,
        Max
    };

private:
    int mn_ImageSize_x;
    int mn_ImageSize_y;
    std::string ms_filename;

    Option m_currentSelect;
    std::optional<Option> m_ConfirmedOption;

public:
    Menu() : GameObject(), ms_filename("Resource/images/MenuBackScreen.png"),
        m_currentSelect(Option::ReturnToTitle)
    {
        // ★画面中央のピクセル座標を指定する（例：1920x1080の画面なら 960, 540）
        SetPosition(Vector2f(960, 540));
        SetTag("Menu");
        mb_IsActive = false;

        int tileHandle = ResourceManager::GetInstance().LoadGraphic(ms_filename);
        auto trans = GetTransform();
        GraphicsManager::GetInstance().GetGraphSize(tileHandle, &mn_ImageSize_x, &mn_ImageSize_y);

        // ★第3引数を true にして UI 用の SpriteComponent として登録
        AddComponent<SpriteComponent>(tileHandle, trans);
    }

    virtual ~Menu() {
        ResourceManager::GetInstance().ReleaseGraphic(ms_filename);
    }

    std::optional<Option> GetConfirmedOption() const { return m_ConfirmedOption; }

    virtual void Update(double deltaTime) override
    {
        m_ConfirmedOption.reset();
        if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_TAB)) {
            mb_IsActive = !mb_IsActive;
            if (mb_IsActive) m_currentSelect = Option::ReturnToTitle;
        }

        if (!mb_IsActive) return;

        // 上下キーでカーソル移動
        if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_UP)) {
            int next = static_cast<int>(m_currentSelect) - 1;
            if (next < 0) next = static_cast<int>(Option::Max) - 1;
            m_currentSelect = static_cast<Option>(next);
        }
        if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_DOWN)) {
            int next = (static_cast<int>(m_currentSelect) + 1) % static_cast<int>(Option::Max);
            m_currentSelect = static_cast<Option>(next);
        }

        // 決定キー（エンターキーなどを想定）
        if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_RETURN)) {
            mb_IsActive = false;

            m_ConfirmedOption = m_currentSelect;
        }

        GameObject::Update(deltaTime);
    }

    virtual void Draw(const Camera& camera) override
    {
        if (!mb_IsActive) return;

        // 1. まず背景画像を描画（SpriteComponent::Draw が呼ばれる）
        GameObject::Draw(camera);

        // 2. カーソルやテキストの描画
        // ※GraphicsManager に DrawString のような文字描画関数があると仮定しています。
        // （DxLibの DrawString をラップしているものと想定）
        int baseX = Config::SCREEN_WIDTH / 2 - 400; // 文字を描画する基準X座標
        int baseY = 450; // 文字を描画する基準Y座標
        int colorWhite = GetColor(255, 255, 255); // DxLibの場合

        // 描画する直前に、システム全体の標準フォントサイズを32に変更
        SetFontSize(96);

        if (m_currentSelect == Option::ReturnToTitle) {
            GraphicsManager::GetInstance().DrawString(baseX, baseY, "→ タイトルへ", colorWhite);
            GraphicsManager::GetInstance().DrawString(baseX, baseY + 150, "　 リスタート", colorWhite);
        }
        else if (m_currentSelect == Option::Restart) {
            GraphicsManager::GetInstance().DrawString(baseX, baseY, "　 タイトルへ", colorWhite);
            GraphicsManager::GetInstance().DrawString(baseX, baseY + 150, "→ リスタート", colorWhite);
        }

        // 描画する直前に、システム全体の標準フォントサイズを32に変更
        SetFontSize(16);
    }
};