#pragma once
#include "engine/objects/GameObject.hpp"
#include "platform/dxlib/GraphicsManager.hpp"
// プレイヤーのヘッダをインクルード（環境に合わせて変更してください）
#include "game/actors/Player.hpp" 
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>

struct TextNode {
    std::string text;
    Vector2f position;
    float currentLength;
    bool isTriggered;
};

class ScatteredTextBackground : public GameObject {
private:
    std::vector<TextNode> m_nodes;
    float m_scrollSpeed;
    float m_triggerDistance;
    float m_typeSpeed;

    // ★プレイヤーの実体が存在するメモリアドレス（ポインタ）を保持する
    const Player* m_targetPlayer;

public:
    // コンストラクタでプレイヤーのポインタを受け取る
    ScatteredTextBackground(const Player* player, float scrollSpeed = 0.5f, float triggerDist = 300.f, float typeSpeed = 20.f)
        : m_targetPlayer(player) // アドレスを保存
        , m_scrollSpeed(scrollSpeed)
        , m_triggerDistance(triggerDist)
        , m_typeSpeed(typeSpeed)
    {
        SetPosition(Vector2f(960.f, 540.f));
        SetZOrder(0.f);
    }

    void AddText(const std::string& str, float x, float y) {
        m_nodes.push_back({ str, Vector2f(x, y), 0.0f, false });
    }

    // ★エラー解消：親クラスに合わせて引数なしのUpdateをオーバーライド
    virtual void Update(double deltaTime) override {
        // プレイヤーのポインタがnullptr（無効なアドレス）なら処理しない（安全対策）
        if (!m_targetPlayer) return;

        // ★エラー解消：保持しているポインタからプレイヤーの実体にアクセスして座標を取得
        Vector2f playerPos = m_targetPlayer->GetPosition();

        for (auto& node : m_nodes) {
            if (node.currentLength < node.text.length()) {
                if (!node.isTriggered) {
                    float dx = playerPos.x - node.position.x;
                    float dy = playerPos.y - node.position.y;
                    float distanceSq = (dx * dx) + (dy * dy);
                    float triggerSq = m_triggerDistance * m_triggerDistance;

                    if (distanceSq <= triggerSq) {
                        node.isTriggered = true;
                    }
                }

                if (node.isTriggered) {
                    node.currentLength += m_typeSpeed * deltaTime;
                }
            }
        }

        GameObject::Update(deltaTime);
    }

    virtual void Draw(const Camera& camera) override {
        Vector2f camPos = camera.GetPosition();
        int color = GetColor(214, 230, 238);

        for (const auto& node : m_nodes) {
            int drawLength = static_cast<int>(node.currentLength);
            if (drawLength <= 0) continue;

            // ★エラー解消：マクロの暴走を防ぐため (std::min) と括弧で囲む
            drawLength = (std::min)(drawLength, static_cast<int>(node.text.length()));

            float drawX = node.position.x - (camPos.x * m_scrollSpeed);
            float drawY = node.position.y - (camPos.y * m_scrollSpeed);

            char buffer[256];
            int copyLength = (std::min)(drawLength, 255);

            // ★エラー解消：安全な strncpy_s を使用し、バッファサイズ(sizeof)を明示する
            strncpy_s(buffer, sizeof(buffer), node.text.c_str(), copyLength);

            // strncpy_s は自動で \0 を付与してくれますが、部分コピーの場合は
            // 指定文字数の直後に確実に \0 を置く必要があります。
            buffer[copyLength] = '\0';

            GraphicsManager::GetInstance().DrawString(
                static_cast<int>(drawX),
                static_cast<int>(drawY),
                buffer,
                color
            );
        }

        GameObject::Draw(camera);
    }
};