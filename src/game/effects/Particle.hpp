#pragma once
#include <DxLib.h>
#include <vector>
#include "engine/scene/Camera.hpp" // ※ご自身の環境に合わせてください

// 色情報を持たせるように追加
struct ParticleData
{
    float x, y;
    float vx, vy;
    int timer;
    int maxTimer; // 初期寿命（フェードアウト計算用）
    unsigned int color; // 色
};

// 砂煙用に追加
enum ParticleType
{
    Random,
    Circle,
    WalkDust, // ★追加：歩行時の砂煙
};

class Particle
{
public:
    void Init();
    void Play(float x, float y, ParticleType t);
    void Update();
    void Draw(const Camera& camera);

private:
    std::vector<ParticleData> particles;
};

inline void Particle::Init()
{
    particles.clear();
}

inline void Particle::Play(float px, float py, ParticleType t)
{
    int count = 0;

    // タイプによって発生させる数を変える
    if (t == Random || t == Circle) count = 20;
    if (t == WalkDust) count = 3; // 砂煙は歩くたびに少しずつ出すので少なめ

    for (int i = 0; i < count; i++)
    {
        ParticleData p;
        p.x = px;
        p.y = py;

        if (t == Random)
        {
            float angle = GetRand(360) * DX_PI_F / 180.0f;
            float speed = GetRand(10) / 2.0f + 1.0f;
            p.vx = cosf(angle) * speed;
            p.vy = sinf(angle) * speed;
            p.timer = 30;
            p.color = GetColor(255, 200, 50); // オレンジ
        }
        else if (t == Circle)
        {
            float angle = (2.0f * DX_PI_F / count) * i;
            float speed = 3.0f;
            p.vx = cosf(angle) * speed;
            p.vy = sinf(angle) * speed;
            p.timer = 30;
            p.color = GetColor(255, 200, 50);
        }
        else if (t == WalkDust)
        {
            // ★砂煙の挙動：上方向にランダムにばらける
            // X方向: -1.0 ～ 1.0
            p.vx = (GetRand(20) - 10) / 10.0f;
            // Y方向: -0.5 ～ -2.5 (マイナス方向＝上へ飛ぶ)
            p.vy = -(GetRand(20) + 5) / 10.0f;

            p.timer = 20 + GetRand(10); // 寿命は20～30フレーム
            // 色は白～薄いグレー
            int c = 200 + GetRand(55);
            p.color = GetColor(c, c, c);
        }

        p.maxTimer = p.timer; // 最大寿命を記録
        particles.push_back(p);
    }
}

inline void Particle::Update()
{
    for (auto& p : particles)
    {
        p.x += p.vx;
        p.y += p.vy;

        // 摩擦（徐々に減速させる）
        p.vx *= 0.9f;
        p.vy *= 0.9f;

        // 重力（砂煙の場合は軽くする、または上へ昇らせる）
        // p.vy += 0.2f; を少し弱くします
        p.vy += 0.05f;

        p.timer--;
    }

    // 消えた粒を削除（Erase-Remove イディオム）
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const ParticleData& p) { return p.timer <= 0; }),
        particles.end()
    );
}

inline void Particle::Draw(const Camera& camera)
{
    for (const auto& p : particles)
    {
        // maxTimer を使って、徐々に透明になるように計算
        int alpha = (int)(255.0f * ((float)p.timer / p.maxTimer));

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

        Vector2f screen = camera.WorldToScreen(Vector2f(p.x, p.y));

        // 砂煙は四角形（Box）で描画すると「ドット絵（ピクセル）」っぽさが出て相性が良いです
        DrawBox(
            (int)screen.x - 2, (int)screen.y - 2,
            (int)screen.x + 2, (int)screen.y + 2,
            p.color, TRUE
        );
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}