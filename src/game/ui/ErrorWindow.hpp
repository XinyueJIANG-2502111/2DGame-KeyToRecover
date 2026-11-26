#pragma once
#include "engine/objects/GameObject.hpp"
#include "game/actors/Player.hpp"
#include "platform/dxlib/GraphicsManager.hpp"
#include "engine/scene/Camera.hpp"
#include <string>

class ErrorWindow : public GameObject {
private:
    enum class State {
        HIDDEN,      // 非表示（待機）
        BLINKING,    // チカチカ点滅中（起動演出）
        VISIBLE      // 完全表示
    };

    State m_State = State::HIDDEN;

    Player* mp_Player;           // プレイヤーのポインタ（距離計算用）
    GameObject* mp_Target;       // 監視対象のオブジェクト（nullptr許容）

    int m_FrameImageHandle;      // ウィンドウ枠の画像ハンドル
    std::string m_Message;       // 表示する文章
    float m_TriggerDistance;     // 反応する距離

    double m_BlinkTimer = 0.0;   // 点滅用のタイマー
    const double BLINK_DURATION = 0.5; // 点滅演出の総時間（秒）
    bool m_IsLightOn = false;    // 点滅時の描画フラグ

    int r, g, b = 0;

public:
    // コンストラクタ
    // pos: 設置座標, dist: 反応距離, msg: メッセージ, imgHandle: 枠画像, player: プレイヤー, target: 監視対象(任意)
    ErrorWindow(const Vector2f& pos, float dist, const std::string& msg, int imgHandle, Player* player, GameObject* target = nullptr)
        : mp_Player(player), mp_Target(target), m_FrameImageHandle(imgHandle), m_Message(msg), m_TriggerDistance(dist)
    {
       SetZOrder(0); // 適切なZOrderを設定
        // GameObjectのコンストラクタでTransformComponentは自動追加済み [cite: 6]
        SetPosition(pos) ;
    }

    void Update(double deltaTime) override {
        GameObject::Update(deltaTime) ;

            // --- 1. 監視対象のメモリ安全確認（最重要） ---
            if (mp_Target) {
                // メモリから実体が消去される前に、削除予約フラグを検知する
                if (mp_Target->IsPendingDestroy()) {
                   
                    mp_Target = nullptr; // 危険なアドレスを手放す
                    Destroy();           // 自身も削除予約を入れる（または SetActive(false) で隠す）[cite: 11]
                    return;
                }
            }

        // --- 2. 距離計算と状態遷移 ---
        if (!mp_Player || !mp_Player->IsActive()) return;

        Vector2f myPos = GetPosition();
            Vector2f playerPos = mp_Player->GetPosition();

        // 簡易的な距離計算 (xの差の2乗 + yの差の2乗)
        float dx = myPos.x - playerPos.x;
        float dy = myPos.y - playerPos.y;
        float distanceSq = (dx * dx) + (dy * dy);
        float triggerSq = m_TriggerDistance * m_TriggerDistance;

        switch (m_State) {
        case State::HIDDEN:
            // プレイヤーが近づいたら蛍光灯のように点滅開始
            if (distanceSq <= triggerSq) {
                m_State = State::BLINKING;
                m_BlinkTimer = 0.0;
            }
            break;

        case State::BLINKING:
            m_BlinkTimer += deltaTime;

            // deltaTimeを使って細かい周期でチカチカさせる（例: 0.05秒ごとに切り替え）
            // fmod は浮動小数点の剰余計算
            m_IsLightOn = (std::fmod(m_BlinkTimer, 0.1) > 0.05);

            // 一定時間経過で完全表示へ
            if (m_BlinkTimer >= BLINK_DURATION) {
                m_State = State::VISIBLE;
            }

            // 途中で離れたら非表示に戻す
            if (distanceSq > triggerSq) {
                m_State = State::HIDDEN;
            }
            break;

        case State::VISIBLE:
            // 離れたら消える
            if (distanceSq > triggerSq) {
                m_State = State::HIDDEN;
            }
            break;
        }
    }

    void Draw(const Camera& camera) override {
        if (!IsActive()) return;

            // --- 3. 状態に応じた描画 ---
            if (m_State == State::HIDDEN || (m_State == State::BLINKING && !m_IsLightOn)) {
                return; // 描画しない
            }

        // SpriteComponentでのUI描画と同様に、カメラの座標変換を行う [cite: 80, 81]
        Vector2f drawPos = camera.WorldToScreen(GetPosition());

        int drawX = static_cast<int>(drawPos.x);
        int drawY = static_cast<int>(drawPos.y);

        // 枠の描画（中心座標などの調整は適宜行ってください）
        GraphicsManager::GetInstance().DrawGraph(drawX, drawY, m_FrameImageHandle, TRUE);

            // テキストの描画 (DxLibのDrawStringを想定。色は適宜設定)
            // 枠からの相対位置（例として +10, +10）に文字を描く
             DrawString(drawX + 100, drawY + 100
                 , m_Message.c_str(), GetColor(r, g, b));
    }

    void SetStringColor(int _r, int _g, int _b) {
        r = _r;
        g = _g;
        b = _b;
    }
};