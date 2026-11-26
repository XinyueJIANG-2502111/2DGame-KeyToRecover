#pragma once

#include "Component.hpp"
#include "TransformComponent.hpp"
#include "platform/dxlib/GraphicsManager.hpp"

// character の画像をコントロールするクラス
// 移動方向によって画像を反転することができる
// アニメーションを追加の場合もこのクラスを使えそう

class SpriteComponent : public Component {
private:
    int m_ImageHandle;                // 読み込んだ画像のハンドル
    TransformComponent* mp_Transform; // transform component
    bool mb_FlipHorizontal;           // （左右）反転する？プレイヤー移動の時使えそう

    int mn_imageSizeX, mn_imageSizeY;
    bool mb_IsUI; // ★追加：UIかどうか（カメラを無視するか）のフラグ

public:
    SpriteComponent(int handle, TransformComponent* transform)
        : m_ImageHandle(handle), mp_Transform(transform), mb_FlipHorizontal(false), mn_imageSizeX(0), mn_imageSizeY(0)
    {
        GraphicsManager::GetInstance().GetGraphSize(handle, &mn_imageSizeX, &mn_imageSizeY);
    }

    void Initialize() override {}
    void Update(double deltaTime) override {}

    void Draw(const Camera& camera) override {
        if (!mp_Transform || m_ImageHandle == -1) return;

        Vector2f drawPos;
        if (mb_IsUI) {
            // ★UIならカメラ変換を無視。Transformの座標をそのまま画面のピクセル座標として使う
            drawPos = mp_Transform->mv_position;
        }
        else {
            // ★通常はワールド座標をスクリーン座標に変換する
            drawPos = camera.WorldToScreen(mp_Transform->mv_position);
        }

        int drawX = (int)drawPos.x - (mn_imageSizeX / 2);
        int drawY = (int)drawPos.y - (mn_imageSizeY / 2);

        GraphicsManager::GetInstance().DrawGraph(drawX, drawY, m_ImageHandle, TRUE);
    }

    void SetImage(int handle) { m_ImageHandle = handle; }

    void SetFlip(bool flip) { mb_FlipHorizontal = flip; }
};