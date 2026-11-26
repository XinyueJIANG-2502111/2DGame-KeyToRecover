#pragma once
#include "engine/components/Component.hpp"
#include "engine/components/TransformComponent.hpp"
#include "engine/scene/Camera.hpp"
#include "engine/math/Vector.hpp"
#include <vector>
#include <algorithm>

class Player;

enum class FaceEffectState {
    FLOATING,
    SCATTER,
    FORMING
};

struct FaceParticle {
    Vector2f pos;
    Vector2f localTarget; // ★変更：顔の中心からの「相対座標」を保持する
    Vector2f velocity;
    float phaseX;
    float phaseY;
};

class BSoDFaceComponent : public Component {
private:
    TransformComponent* mp_Transform;
    Player* mp_Player;

    std::vector<FaceParticle> m_particles;
    FaceEffectState m_state;
    float m_stateTimer;
    float m_globalTime;
    int m_currentPatternIndex;

    std::vector<Vector2f> GeneratePattern(int patternIndex);
    void UpdateTargetPositions();

public:
    BSoDFaceComponent(TransformComponent* transform, Player* player);
    virtual ~BSoDFaceComponent() = default;

    void Initialize() override;
    void Update(double deltaTime) override;
    void Draw(const Camera& camera) override;
};

#include "game/actors/Player.hpp" 
#include <random>
#include <cmath>

#define NOMINMAX 
#include <windows.h>
#undef max
#undef min
#include "DxLib.h"

BSoDFaceComponent::BSoDFaceComponent(TransformComponent* transform, Player* player)
    : mp_Transform(transform), mp_Player(player), m_state(FaceEffectState::FORMING),
    m_stateTimer(0.0f), m_globalTime(0.0f), m_currentPatternIndex(0)
{
    const int PARTICLE_COUNT = 1000;
    std::random_device rd;
    std::mt19937 gen(rd());

    // 背景として広範囲に散らす
    std::uniform_real_distribution<float> distSpawn(0.0f, (float)Config::SCREEN_WIDTH);
    std::uniform_real_distribution<float> distSpawnY(0.0f, (float)Config::SCREEN_HEIGHT);
    std::uniform_real_distribution<float> distPhase(0.0f, 3.141592f * 2.0f);

    m_particles.reserve(PARTICLE_COUNT);
    for (int i = 0; i < PARTICLE_COUNT; ++i) {
        FaceParticle p;
        p.pos = Vector2f(distSpawn(gen), distSpawnY(gen));
        p.localTarget = Vector2f(0.f, 0.f);
        p.velocity = Vector2f(0.f, 0.f);
        p.phaseX = distPhase(gen);
        p.phaseY = distPhase(gen);
        m_particles.push_back(p);
    }

    UpdateTargetPositions();
}

void BSoDFaceComponent::Initialize() {}

std::vector<Vector2f> BSoDFaceComponent::GeneratePattern(int patternIndex) {
    std::vector<Vector2f> pattern;
    const int MULT = 1;

    if (patternIndex == 0) {
        // 0: (・_・) 無表情・監視（計 10,000個）

        // 1. 左の輪郭「 ( 」: 2500個
        int count1 = 250 * MULT;
        for (int i = 0; i < count1; ++i) {
            float t = static_cast<float>(i) / (count1 - 1.0f);
            pattern.push_back(Vector2f(-250.0f - 50.0f * std::sin(t * 3.141592f), -150.0f + 300.0f * t));
        }
        // 2. 右の輪郭「 ) 」: 2500個
        int count2 = 250 * MULT;
        for (int i = 0; i < count2; ++i) {
            float t = static_cast<float>(i) / (count2 - 1.0f);
            pattern.push_back(Vector2f(250.0f + 50.0f * std::sin(t * 3.141592f), -150.0f + 300.0f * t));
        }
        // 3. 左目「・」: 1500個
        int count3 = 150 * MULT;
        for (int i = 0; i < count3; ++i) {
            float t = static_cast<float>(i) / count3;
            float rad = t * 3.141592f * 2.0f;
            pattern.push_back(Vector2f(-100.0f + 15.0f * std::cos(rad), -30.0f + 15.0f * std::sin(rad)));
        }
        // 4. 右目「・」: 1500個
        int count4 = 150 * MULT;
        for (int i = 0; i < count4; ++i) {
            float t = static_cast<float>(i) / count4;
            float rad = t * 3.141592f * 2.0f;
            pattern.push_back(Vector2f(100.0f + 15.0f * std::cos(rad), -30.0f + 15.0f * std::sin(rad)));
        }
        // 5. 口「_」: 2000個
        int count5 = 200 * MULT;
        for (int i = 0; i < count5; ++i) {
            float t = static_cast<float>(i) / (count5 - 1.0f);
            pattern.push_back(Vector2f(-40.0f + 80.0f * t, 50.0f));
        }
    }
    else if (patternIndex == 1) {
        // 1: (≖_≖ ) 疑心暗鬼・ジト目（計 10,000個）

        // 輪郭（左右）: 各2000個
        int count1 = 200 * MULT;
        for (int i = 0; i < count1; ++i) {
            float t = static_cast<float>(i) / (count1 - 1.0f);
            pattern.push_back(Vector2f(-260.0f - 50.0f * std::sin(t * 3.141592f), -150.0f + 300.0f * t)); // 左
            pattern.push_back(Vector2f(260.0f + 50.0f * std::sin(t * 3.141592f), -150.0f + 300.0f * t)); // 右
        }
        // 左目「≖」（上まぶた1000個、瞳500個）
        int count2 = 100 * MULT;
        int count3 = 50 * MULT;
        for (int i = 0; i < count2; ++i) {
            float t = static_cast<float>(i) / (count2 - 1.0f);
            pattern.push_back(Vector2f(-150.0f + 80.0f * t, -40.0f)); // 上まぶた（直線）
        }
        for (int i = 0; i < count3; ++i) {
            float rad = (static_cast<float>(i) / count3) * 3.141592f * 2.0f;
            pattern.push_back(Vector2f(-110.0f + 10.0f * std::cos(rad), -10.0f + 10.0f * std::sin(rad))); // 瞳
        }
        // 右目「≖」（上まぶた1000個、瞳500個）
        for (int i = 0; i < count2; ++i) {
            float t = static_cast<float>(i) / (count2 - 1.0f);
            pattern.push_back(Vector2f(70.0f + 80.0f * t, -40.0f));
        }
        for (int i = 0; i < count3; ++i) {
            float rad = (static_cast<float>(i) / count3) * 3.141592f * 2.0f;
            pattern.push_back(Vector2f(110.0f + 10.0f * std::cos(rad), -10.0f + 10.0f * std::sin(rad)));
        }
        // 口「_」: 2000個
        int count4 = 200 * MULT;
        for (int i = 0; i < count4; ++i) {
            float t = static_cast<float>(i) / (count4 - 1.0f);
            pattern.push_back(Vector2f(-30.0f + 60.0f * t, 50.0f));
        }
    }
    else if (patternIndex == 2) {
        // 2: ( ｀ー´) 怒り・強気（計 10,000個）

        // 輪郭（左右）: 各2500個
        int count1 = 250 * MULT;
        for (int i = 0; i < count1; ++i) {
            float t = static_cast<float>(i) / (count1 - 1.0f);
            pattern.push_back(Vector2f(-280.0f - 50.0f * std::sin(t * 3.141592f), -150.0f + 300.0f * t));
            pattern.push_back(Vector2f(280.0f + 50.0f * std::sin(t * 3.141592f), -150.0f + 300.0f * t));
        }
        // 左目「｀」（吊り上がった眉目）: 1000個
        int count2 = 100 * MULT;
        for (int i = 0; i < count2; ++i) {
            float t = static_cast<float>(i) / (count2 - 1.0f);
            pattern.push_back(Vector2f(-140.0f + 40.0f * t, -80.0f + 60.0f * t));
        }
        // 右目「´」: 1000個
        for (int i = 0; i < count2; ++i) {
            float t = static_cast<float>(i) / (count2 - 1.0f);
            pattern.push_back(Vector2f(100.0f + 40.0f * t, -20.0f - 60.0f * t));
        }
        // 口「ー」（長めの直線）: 3000個
        int count3 = 300 * MULT;
        for (int i = 0; i < count3; ++i) {
            float t = static_cast<float>(i) / (count3 - 1.0f);
            pattern.push_back(Vector2f(-80.0f + 160.0f * t, 40.0f));
        }
    }
    else if (patternIndex == 3) {
        // 3: ( ﾟДﾟ) 驚き・叫び（計 10,000個）

        // 輪郭（左右）: 各2000個
        int count1 = 200 * MULT;
        for (int i = 0; i < count1; ++i) {
            float t = static_cast<float>(i) / (count1 - 1.0f);
            pattern.push_back(Vector2f(-250.0f - 50.0f * std::sin(t * 3.141592f), -150.0f + 300.0f * t));
            pattern.push_back(Vector2f(250.0f + 50.0f * std::sin(t * 3.141592f), -150.0f + 300.0f * t));
        }
        // 目「ﾟ」（小さな円、左右各1000個）
        int count2 = 100 * MULT;
        for (int i = 0; i < count2; ++i) {
            float rad = (static_cast<float>(i) / count2) * 3.141592f * 2.0f;
            pattern.push_back(Vector2f(-120.0f + 15.0f * std::cos(rad), -70.0f + 15.0f * std::sin(rad))); // 左
            pattern.push_back(Vector2f(120.0f + 15.0f * std::cos(rad), -70.0f + 15.0f * std::sin(rad)));  // 右
        }
        // 口「Д」（台形：上下左右の4本の線、各1000個で計4000個）
        int count3 = 100 * MULT;
        for (int i = 0; i < count3; ++i) {
            float t = static_cast<float>(i) / (count3 - 1.0f);
            pattern.push_back(Vector2f(-50.0f + 100.0f * t, -10.0f));                 // 上辺
            pattern.push_back(Vector2f(-70.0f + 140.0f * t, 70.0f));                  // 下辺
            pattern.push_back(Vector2f(-50.0f - 20.0f * t, -10.0f + 80.0f * t));      // 左辺
            pattern.push_back(Vector2f(50.0f + 20.0f * t, -10.0f + 80.0f * t));       // 右辺
        }
    }
    else {
        // 4: :( 最終エラー・BSoD（計 10,000個）

        // コロンの上の目: 2000個
        int count1 = 200 * MULT;
        for (int i = 0; i < count1; ++i) {
            float rad = (static_cast<float>(i) / count1) * 3.141592f * 2.0f;
            pattern.push_back(Vector2f(-120.0f + 20.0f * std::cos(rad), -80.0f + 20.0f * std::sin(rad)));
        }
        // コロンの下の目: 2000個
        for (int i = 0; i < count1; ++i) {
            float rad = (static_cast<float>(i) / count1) * 3.141592f * 2.0f;
            pattern.push_back(Vector2f(-120.0f + 20.0f * std::cos(rad), 80.0f + 20.0f * std::sin(rad)));
        }
        // 悲しい口「 ( 」（左に膨らむ大きなカーブ）: 6000個
        int count2 = 600 * MULT;
        for (int i = 0; i < count2; ++i) {
            float t = static_cast<float>(i) / (count2 - 1.0f);
            float y = -150.0f + 300.0f * t;
            float x = 160.0f - 100.0f * std::sin(t * 3.141592f); // マイナス方向(左)に弧を描く
            pattern.push_back(Vector2f(x, y));
        }
    }

    // ========================================================
    // ★ 最後に全頂点を一括で拡大（スケーリング）
    // ========================================================
    const float SCALE = 2.5f; // 画面サイズに合わせて微調整してください
    for (auto& pos : pattern) {
        pos.x *= SCALE;
        pos.y *= SCALE;
    }

    return pattern;
}

void BSoDFaceComponent::UpdateTargetPositions() {
    std::vector<Vector2f> baseTargets = GeneratePattern(m_currentPatternIndex);
    if (baseTargets.empty()) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distOffset(-30.0f, 30.0f); // 巨大化に合わせてばらつきも大きく

    for (size_t i = 0; i < m_particles.size(); ++i) {
        int targetIndex = i % baseTargets.size();
        m_particles[i].localTarget = Vector2f(
            baseTargets[targetIndex].x + distOffset(gen),
            baseTargets[targetIndex].y + distOffset(gen)
        );
    }
}

void BSoDFaceComponent::Update(double deltaTime) {
    if (!mp_Player) return;

    m_globalTime += (float)deltaTime;

    // ★座標監視：1000進むごとに自動でSCATTER
    float playerX = (std::max)(0.0f, mp_Player->GetPosition().x);
    int targetPatternIndex = static_cast<int>(playerX / 1500.0f);
    targetPatternIndex = (std::max)(0, (std::min)(targetPatternIndex, 4));

    if (targetPatternIndex != m_currentPatternIndex && m_state != FaceEffectState::SCATTER) {
        m_currentPatternIndex = targetPatternIndex;
        m_state = FaceEffectState::SCATTER;
        m_stateTimer = 0.0f;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distExplode(-600.0f, 600.0f);
        for (auto& p : m_particles) {
            p.velocity = Vector2f(distExplode(gen), distExplode(gen));
        }
    }

    if (m_state == FaceEffectState::SCATTER) {
        m_stateTimer += (float)deltaTime;
        if (m_stateTimer > 1.0f) {
            m_state = FaceEffectState::FORMING;
            UpdateTargetPositions();
        }
    }

    // ★背景固定：画面中央を基準点にする
    Vector2f screenCenter((float)Config::SCREEN_WIDTH / 2, (float)Config::SCREEN_HEIGHT / 2);

    for (auto& p : m_particles) {
        switch (m_state) {
        case FaceEffectState::SCATTER:
            p.pos.x += p.velocity.x * (float)deltaTime;
            p.pos.y += p.velocity.y * (float)deltaTime;
            p.velocity.x *= 0.95f;
            p.velocity.y *= 0.95f;
            break;

        case FaceEffectState::FORMING:
            // キモくない程度の揺らぎ（背景なのでゆっくり大きく動かす）
            float waveX = std::sin(m_globalTime * 1.0f + p.phaseX) * 12.0f;
            float waveY = std::sin(m_globalTime * 0.8f + p.phaseY) * 12.0f;

            // 目標座標 = 画面中央 + パターンの相対位置 + 揺らぎ
            Vector2f currentTarget(
                screenCenter.x + p.localTarget.x + waveX,
                screenCenter.y + p.localTarget.y + waveY
            );

            // Lerpで滑らかに移動
            p.pos.x += (currentTarget.x - p.pos.x) * 4.0f * (float)deltaTime;
            p.pos.y += (currentTarget.y - p.pos.y) * 4.0f * (float)deltaTime;
            break;
        }
    }
}

void BSoDFaceComponent::Draw(const Camera& camera) {
    // ★カメラを無視して直接スクリーン座標で描画

    for (const auto& p : m_particles) {
        DrawBox(
            (int)p.pos.x, (int)p.pos.y,
            (int)p.pos.x + 12, (int)p.pos.y + 12, // ■のサイズも少し大きく
            GetColor(120, 153, 195), TRUE
        );
    }
}