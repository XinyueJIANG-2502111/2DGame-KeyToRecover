#pragma once
#include "engine/objects/GameObject.hpp"
#include "engine/scene/Camera.hpp"

class GlitchBackgroundObject : public GameObject {
private:
    int m_imageHandle;
    int m_width;
    int m_height;
    int m_timer;

public:
    // コンストラクタで画像ハンドルを受け取る
    GlitchBackgroundObject(int imageHandle);
    virtual ~GlitchBackgroundObject() = default;

    void Update(double deltaTime) override;
    void Draw(const Camera& camera) override;
};

#include <DxLib.h>
#include <cstdlib>

GlitchBackgroundObject::GlitchBackgroundObject(int imageHandle)
    : m_imageHandle(imageHandle), m_timer(0)
{
    // コンストラクタ内で一番奥の背景としてZオーダーを設定
    SetZOrder(-1.0f);

    if (m_imageHandle != -1) {
        GetGraphSize(m_imageHandle, &m_width, &m_height);
    }
}

void GlitchBackgroundObject::Update(double deltaTime) {
    GameObject::Update(deltaTime); // 基底クラスのUpdate
    m_timer++;
}

void GlitchBackgroundObject::Draw(const Camera& camera) {
    if (!mb_IsActive || m_imageHandle == -1) return;

    // 自身の座標を取得し、スクリーン座標へ
    Vector2f screenPos = camera.WorldToScreen(GetPosition());
    int x = (int)screenPos.x;
    int y = (int)screenPos.y;

    int glitchType = rand() % 100;

    if (glitchType < 8) {
        // パターン1: 激しい色ズレ（RGBシフト / 色収差）
        int shiftX = (rand() % 40) - 20;
        int shiftY = (rand() % 10) - 5;

        // R（赤）
        SetDrawBright(255, 0, 0);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
        DrawGraph(x + shiftX, y + shiftY, m_imageHandle, TRUE);

        // G（緑）
        SetDrawBright(0, 255, 0);
        SetDrawBlendMode(DX_BLENDMODE_ADD, 200);
        DrawGraph(x - shiftX, y - shiftY, m_imageHandle, TRUE);

        // B（青）
        SetDrawBright(0, 0, 255);
        DrawGraph(x + shiftX / 2, y, m_imageHandle, TRUE);

        // デフォルトに戻す
        SetDrawBright(255, 255, 255);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    }
    else if (glitchType < 25) {
        // パターン2: テアリング（スライス）
        DrawGraph(x, y, m_imageHandle, TRUE);

        int sliceY = rand() % m_height;
        int sliceH = rand() % 50 + 10;
        int shiftX = (rand() % 80) - 40;

        DrawRectGraph(
            x + shiftX, y + sliceY,
            0, sliceY,
            m_width, sliceH,
            m_imageHandle, TRUE, FALSE
        );

    }
    else {
        // パターン3: 通常描画
        DrawGraph(x, y, m_imageHandle, TRUE);
    }

    // オマケ: 走査線ノイズ
    if (rand() % 10 == 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80 + rand() % 80);
        int lineY = y + (rand() % m_height);
        int lineH = rand() % 5 + 1;
        DrawBox(x, lineY, x + m_width, lineY + lineH, GetColor(220, 220, 255), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}