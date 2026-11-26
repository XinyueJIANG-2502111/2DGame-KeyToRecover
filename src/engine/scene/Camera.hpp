#pragma once

#include "engine/math/Vector.hpp"
#include <algorithm>
#include <cstdlib> // rand() を使用するため追加

// カメラ：ワールド座標（例えば背景サイズ）からスクリーン座標（画面サイズ）に変換する
// 設定した目標を追従することができる
// 使い方例：TestSceneをご確認ください
// Draw 関数で何かを描画するとき camera.WorldToScreen(Vector2f worldPos) を使って座標変換することが必要
class Camera {
private:
    Vector2f mv_Position;
    int mn_ScreenWidth;
    int mn_ScreenHeight;

    float mf_DeadzoneWidth;
    float mf_DeadzoneHeight;

    float mf_MapWidth = 0.0f;
    float mf_MapHeight = 0.0f;
    bool  mb_UseBounds = false;

    // ==========================================
    // 画面揺れ（スクリーンシェイク）用の変数
    // ==========================================
    float mf_ShakeIntensity = 0.0f; // 現在の揺れの強さ（ピクセル幅）
    float mf_ShakeDecay = 0.0f;     // 毎フレームの減衰量（揺れが収まるスピード）
    Vector2f mv_ShakeOffset = { 0.0f, 0.0f }; // 最終的な描画をズラすための内部数値

public:
    Camera(int w, int h) : mn_ScreenWidth(w), mn_ScreenHeight(h), mv_Position(0, 0) {}

    // 境界設定
    void SetBounds(float mapWidth, float mapHeight) {
        mf_MapWidth = mapWidth;
        mf_MapHeight = mapHeight;
        mb_UseBounds = true;
    }

    // デッドゾーン設定（プレイヤーがこの枠から出るとカメラはプレイヤーに追従する）
    void SetDeadzone(float w, float h) {
        mf_DeadzoneWidth = w;
        mf_DeadzoneHeight = h;
    }

    // ==========================================
    // 画面揺れを開始する関数
    // ==========================================
    // intensity: 揺れの初期の強さ（例：15.0f）
    // decay: 毎フレーム減らしていく値（例：0.5f）
    void StartShake(float intensity, float decay) {
        mf_ShakeIntensity = intensity;
        mf_ShakeDecay = decay;
    }

    void Update(const Vector2f& targetPos, float lerpFactor = 0.2f) {
        float deadzoneLeft = mv_Position.x - (mf_DeadzoneWidth * 0.5f);
        float deadzoneRight = mv_Position.x + (mf_DeadzoneWidth * 0.5f);
        float deadzoneTop = mv_Position.y - (mf_DeadzoneHeight * 0.5f);
        float deadzoneBottom = mv_Position.y + (mf_DeadzoneHeight * 0.5f);

        if (targetPos.x < deadzoneLeft) {
            mv_Position.x -= (deadzoneLeft - targetPos.x);
        }
        else if (targetPos.x > deadzoneRight) {
            mv_Position.x += (targetPos.x - deadzoneRight);
        }

        if (targetPos.y < deadzoneTop) {
            mv_Position.y -= (deadzoneTop - targetPos.y);
        }
        else if (targetPos.y > deadzoneBottom) {
            mv_Position.y += (targetPos.y - deadzoneBottom);
        }

        if (mb_UseBounds) {
            float halfWinW = mn_ScreenWidth * 0.5f;
            float halfWinH = mn_ScreenHeight * 0.5f;

            if (mf_MapWidth > mn_ScreenWidth) {
                mv_Position.x = std::clamp(mv_Position.x, halfWinW, mf_MapWidth - halfWinW);
            }
            if (mf_MapHeight > mn_ScreenHeight) {
                mv_Position.y = std::clamp(mv_Position.y, halfWinH, mf_MapHeight - halfWinH);
            }
        }

        // ==========================================
        // 画面揺れの更新処理
        // ==========================================
        if (mf_ShakeIntensity > 0.0f) {
            // -1.0 ～ 1.0 のランダムな数値を生成
            float randX = (rand() % 200 - 100) / 100.0f;
            float randY = (rand() % 200 - 100) / 100.0f;

            // 強さに応じてランダムな方向にオフセットを決定
            mv_ShakeOffset.x = randX * mf_ShakeIntensity;
            mv_ShakeOffset.y = randY * mf_ShakeIntensity;

            // 揺れを減衰させる
            mf_ShakeIntensity -= mf_ShakeDecay;
            if (mf_ShakeIntensity <= 0.0f) {
                mf_ShakeIntensity = 0.0f;
                mv_ShakeOffset = { 0.0f, 0.0f }; // 揺れが完全に止まったらズレを0に戻す
            }
        }
        else {
            mv_ShakeOffset = { 0.0f, 0.0f };
        }
    }

    // 追従するものを設定する
    void LookAt(Vector2f target) {
        mv_Position = target;
    }

    // 座標変換
    Vector2f WorldToScreen(Vector2f worldPos) const {
        return {
            // ★ mv_Position に mv_ShakeOffset を足して、画面への出力結果だけをズラす
            worldPos.x - (mv_Position.x + mv_ShakeOffset.x) + mn_ScreenWidth * 0.5f,
            worldPos.y - (mv_Position.y + mv_ShakeOffset.y) + mn_ScreenHeight * 0.5f
        };
    }

    // 座標設定・獲得
    Vector2f GetPosition() const { return mv_Position; }
    void SetPosition(const Vector2f& pos) { mv_Position = pos; }
};