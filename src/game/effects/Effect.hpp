#pragma once
#include<DxLib.h>
#include<vector>
#include"engine/scene/Camera.hpp"

enum EffectType
{
    Effect_Attack,
    Effect_Electric, // 感電エフェクト
    Effect_Max
};
class Effect
{

public:
    void Init();                        // 初期化
    void Play(float x, float y, EffectType t);     // エフェクト発動
    void Update(float px, float py);                    // 更新
    void Draw(const Camera& camera);                     // 描画

private:
    int x = 0;
    int y = 0;
    int timer = 0;
    bool isActive = false;
    int type = 0;
    int img[Effect_Max] = {};
};

inline void Effect::Init()
{
    img[Effect_Attack] = LoadGraph(_T("Resource/images/effeckt.png"));
    img[Effect_Electric] = LoadGraph(_T("Resource/images/Erector.PNG"));
    isActive = false;
}

inline void Effect::Play(float px, float py, EffectType t)
{
    x = px;
    y = py;
    timer = 30;
    isActive = true;
    type = t;
}

inline void Effect::Update(float px, float py)
{
    if (!isActive) return;

    timer--;

    x = px;
    y = py;

    if (timer <= 0)
    {
        isActive = false;
    }
}

inline void Effect::Draw(const Camera& camera)
{
    if (!isActive) return;

    Vector2f screen =
        camera.WorldToScreen(Vector2f(x, y));

    // 感電エフェクトの場合、座標をランダムにずらす
    float drawX = screen.x;
    float drawY = screen.y;

    if (type == Effect_Electric)
    {
        SetDrawBlendMode(DX_BLENDMODE_ADD, 255); // 加算合成
    }

    if (type == Effect_Electric)
    {
        // 毎フレーム-4 ～ 4 ピクセルの範囲でランダムに揺らす
        drawX += GetRand(8) - 4;
        drawY += GetRand(8) - 4;
    }



    DrawRotaGraph(
        //screen.x,
        //screen.y,

        (int)drawX,
        (int)drawY,
        1.0,
        0.0,
        img[type],
        TRUE
    );

    if (type == Effect_Electric)
    {
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255); // 加算合成
    }
}