#pragma once
#include "engine/objects/GameObject.hpp"
#include "engine/math/Vector.hpp"
#include <vector>

// ブロック1つ分の実体データ（以前と同じ）
struct FloatingBlock {
    Vector2f basePos;
    Vector2f currentPos;
    float angle;
    float rotSpeed;
    float phase;
    float floatSpeed;
    float amplitude;
    float scale;
};

// Componentではなく、GameObjectそのものを継承する
class FloatingBlocksObject : public GameObject {
private:
    int m_imageHandle;
    std::vector<FloatingBlock> m_blocks;
    float m_globalTime;

public:
    FloatingBlocksObject();
    virtual ~FloatingBlocksObject() = default;

    // GameObjectの仮想関数をオーバーライドして直接処理を書く
    void Update(double deltaTime) override;
    void Draw(const Camera& camera) override;
};

#include "platform/dxlib/ResourceManager.hpp"
#include <random>
#include <cmath>
#include "DxLib.h"

// ブロックの配置データを定義するための構造体
struct BlockData {
    float x, y;        // 配置するX, Y座標
    float scale;       // 大きさ（1.0が等倍）
    float angleDeg;    // 角度（度数法：0〜360）
};

FloatingBlocksObject::FloatingBlocksObject()
    : m_imageHandle(-1), m_globalTime(0.0f)
{
    SetZOrder(-1.0f); // UIより奥に配置

    // 分離したブロック画像を読み込む
    m_imageHandle = ResourceManager::GetInstance().LoadGraphic("Resource/images/botan.PNG");

    // ========================================================
    // ★ ここでブロックの配置（画像1枚目準拠）を手動設定します
    // ※ 座標(x,y)は 1920x1080 などを想定した仮の数値です。
    // 実際の画面サイズに合わせて微調整してください。
    // ========================================================
    std::vector<BlockData> initialData = {
        // --- 左側エリア（X: 100〜450） ---
        { 150.f, 850.f, 0.8f, 25.f },  // 左下
        { 320.f, 650.f, 0.5f, 15.f },  // 左中央下
        { 150.f, 400.f, 0.7f, 45.f },  // 左中央
        { 250.f, 180.f, 0.5f, 60.f },  // 左上
        { 400.f, 100.f, 0.3f, 20.f },  // 左上（内側ギリギリ）

        // --- 右側エリア（X: 1450〜1800） ---
        { 1550.f, 180.f, 0.6f, 45.f }, // 右上（内側）
        { 1750.f, 100.f, 0.5f, 15.f }, // 右上（外側）
        { 1650.f, 350.f, 1.2f, 30.f }, // 右中央（見切れてる巨大なやつ）
        { 1500.f, 500.f, 0.4f, 0.f },  // 右中央（回転なし）
        { 1700.f, 650.f, 0.6f, 45.f }, // 右下（外側）
        { 1550.f, 850.f, 0.7f, 30.f }, // 右下（内側）

        // --- 上端・下端の隙間エリア ---
        { 960.f, 60.f, 0.3f, 10.f }, // 画面上端（「Key」のさらに上）
        { 1200.f, 80.f, 0.4f, 75.f }  // 画面右上端
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distPhase(0.0f, 3.141592f * 2.0f);
    std::uniform_real_distribution<float> distFloatSpeed(0.8f, 1.5f); // 浮く速度に少しバラつきを持たせる

    m_blocks.reserve(initialData.size());

    // リストのデータをもとに実体を生成
    for (const auto& data : initialData) {
        FloatingBlock b;
        b.basePos = Vector2f(data.x, data.y);
        b.currentPos = b.basePos;

        // 度数法(度)からラジアンに変換してセット
        b.angle = data.angleDeg * (3.141592f / 180.0f);

        b.rotSpeed = 0.0f; // ★回転はさせず、指定した角度で固定する

        b.phase = distPhase(gen); // 浮き沈みのタイミング（ズレ）はランダムにする
        b.floatSpeed = distFloatSpeed(gen);
        b.amplitude = 15.0f * data.scale; // 大きなブロックほど揺れ幅を大きくする
        b.scale = data.scale;

        m_blocks.push_back(b);
    }
}

void FloatingBlocksObject::Update(double deltaTime) {
    GameObject::Update(deltaTime);
    m_globalTime += (float)deltaTime;

    for (auto& b : m_blocks) {
        // angle += rotSpeed は 0 なので省略可能

        // プワプワ浮く処理（サイン波を用いたY座標の計算）
        float wave = std::sin(m_globalTime * b.floatSpeed + b.phase) * b.amplitude;

        // X座標と角度は固定、Y座標だけ波のように変化させる
        b.currentPos.x = b.basePos.x;
        b.currentPos.y = b.basePos.y + wave;
    }
}

void FloatingBlocksObject::Draw(const Camera& camera) {
    if (!mb_IsActive || m_imageHandle == -1) return;

    for (const auto& b : m_blocks) {
        DrawRotaGraph(
            (int)b.currentPos.x,
            (int)b.currentPos.y,
            (double)b.scale,
            (double)b.angle,
            m_imageHandle,
            TRUE // 透過有効
        );
    }
}