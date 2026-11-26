#pragma once

# include "engine/scene/Scene.hpp"
# include "Character.hpp"
# include "engine/objects/GameObject.hpp"
# include "engine/objects/ObjectPool.hpp"
#include "engine/scene/Camera.hpp"

# include "game/objects/UnlockItem.hpp"
# include "engine/math/Bounds2.hpp"

# include "platform/input/InputManager.hpp"
# include "platform/input/InputMapping.hpp"

# include "platform/dxlib/GraphicsManager.hpp"
# include "platform/dxlib/ResourceManager.hpp"
# include "game/PlayerProgress.hpp"
# include "platform/dxlib/AudioManager.hpp"

# include "engine/components/PhysicsComponent.hpp"
# include "engine/components/SpriteComponent.hpp"
# include "engine/components/ColliderComponent.hpp"
# include "StateComponent.hpp"

# include "game/effects/Effect.hpp"
# include "game/effects/Particle.hpp"

# include <algorithm>
# include <iostream>


class Player : public Character
{
private:
    // 画像サイズ
    int mn_ImageSize_x;
    int mn_ImageSize_y;

    // 移動
    float mf_Speed = 70.f;
    float mf_JumpForce = -80.0f;

    // 攻撃範囲
    float mf_AttackRange = 70.0f;
    float mf_AttackHeight = 5.0f;
    Bounds2 attackCollider; // test

    // 加速
    float mf_dash_factor = 1.f;

    // 持っているオブジェクトに指すポインター
    GameObject* mp_HeldObject = nullptr;
    float mf_InteractRange = 60.0f;

    // プレイヤーの向き
    bool mb_FacingRight = true;

    // 解放されたアクションを保存するセット
    PlayerProgress& m_Progress;

    Effect effect;
    Particle particle;
    int blinkTimer = 60;
    bool isBlinking = false;

    // ★追加：歩行エフェクト（砂煙）の発生間隔を計るタイマー
    int walkTimer = 0;

public:
    Player(float x, float y, PlayerProgress& progress)
        : Character(x, y), mn_ImageSize_x(0), mn_ImageSize_y(0), m_Progress(progress)
    {
        this->SetTag("Player");

        auto trans = GetTransform();
        int playerHandle = ResourceManager::GetInstance().LoadGraphic("Resource/images/Player.png");

        GraphicsManager::GetInstance().GetGraphSize(playerHandle, &mn_ImageSize_x, &mn_ImageSize_y);

        AddComponent<SpriteComponent>(playerHandle, trans);
        AddComponent<PhysicsComponent>(75.0f);
        AddComponent<ColliderComponent>(trans, Vector2f(mn_ImageSize_x, mn_ImageSize_y));

        effect.Init();
    }

    ~Player() {
        ResourceManager::GetInstance().ReleaseGraphic("Resource/images/Player.png");
    }

    // アイテムから呼ばれる解放関数
    bool IsActionUnlocked(InputAction action) const {
        return m_Progress.IsActionUnlocked(action);
    }

    void TakeDamage(int damage) override {
        m_Progress.currentHp -= damage;
        isBlinking = true;
        effect.Play(GetTransform()->mv_position.x, GetTransform()->mv_position.y, Effect_Electric);
        blinkTimer = 60;

        if (auto* scene = GetScene()) {
            if (auto* camera = scene->GetCamera()) {
                camera->StartShake(15.0f, 0.5f);
            }
        }
    }

    void Update(double deltaTime) override {
        Move(deltaTime);  // 移動
        FacingRight();    // 向きを判断

        if (mp_HeldObject) {
            // 持っているときオブジェクトをプレイヤーの上にのせる
            Vector2f holdOffset(0.0f, -40.0f);
            mp_HeldObject->SetPosition(this->GetPosition() + holdOffset);
        }

        // 攻撃
        if (InputMapping::GetInstance().IsJustPressed(InputAction::ATTACK)
            && IsActionUnlocked(InputAction::ATTACK)) {
            Attack();
            m_Progress.inputCount++; // 20260508
            AudioManager::GetInstance()->PlaySound("Resource/sounds/SE/からぶった.mp3");
        }

        // アイテムとの衝突をチェックして能力を解放する
        for (auto* object : GetScene()->FindObjectsWithTag("UnlockItem")) {
            auto* item = static_cast<UnlockItem*>(object);
            if (GetComponent<ColliderComponent>()->CheckCollision(item->GetComponent<ColliderComponent>())) {
                m_Progress.CollectAction(item->GetInputAction());
                item->Destroy();
                // 取得パーティクル
                particle.Play(GetTransform()->mv_position.x, GetTransform()->mv_position.y, Random); // ※ParticlrTypeのスペルミス修正済み想定
                particle.Play(GetTransform()->mv_position.x, GetTransform()->mv_position.y, Random);
                particle.Play(GetTransform()->mv_position.x, GetTransform()->mv_position.y, Circle);
                AudioManager::GetInstance()->PlaySound("Resource/sounds/SE/取得.mp3");
            }
        }

        if (isBlinking)
        {
            blinkTimer--;
            if (blinkTimer <= 0)
            {
                isBlinking = false;
            }
        }

        effect.Update(GetTransform()->mv_position.x, GetTransform()->mv_position.y);
        particle.Update();

        GameObject::Update(deltaTime);
    }

    void Draw(const Camera& camera) override
    {
        if (!mb_IsActive) return;

        DrawHPBar();

        // テスト用：攻撃時のコライダーを描く
        attackCollider.DrawAABB(camera);
        attackCollider.p_min = 0.f;
        attackCollider.p_max = 0.f;
        attackCollider.DrawAABB(camera);

        if (blinkTimer)
        {
            if ((blinkTimer / 5) % 2 == 0)
            {
                GameObject::Draw(camera);
            }
        }
        else {
            GameObject::Draw(camera);
        }
        effect.Draw(camera);
        particle.Draw(camera);
    }

    // プレイヤーの向き（右か左か）
    void FacingRight() {
        if (this->GetComponent<PhysicsComponent>()->GetVelocity().x < 0) { mb_FacingRight = false; }
        else { mb_FacingRight = true; }
    }

    // プレイヤーの移動処理
    void Move(float deltaTime) {

        mf_dash_factor = 1.f;
        auto* physComp = GetComponent<PhysicsComponent>();

        if (InputMapping::GetInstance().IsPressed(InputAction::DASH)) {
            mf_dash_factor = 2.f;
        }

        if (!physComp) {
            std::cerr << "Warning: Player has no PhysicsComponent!" << std::endl;
            return;
        }

        bool isWalking = false; // ★追加：歩いているかどうかのフラグ

        // MOVE
        if (InputMapping::GetInstance().IsPressed(InputAction::MOVE_RIGHT)
            && IsActionUnlocked(InputAction::MOVE_RIGHT)) {
            physComp->SetVelocity({ mf_Speed * mf_dash_factor, physComp->GetVelocity().y });
            isWalking = true;
        }
        if (InputMapping::GetInstance().IsPressed(InputAction::MOVE_LEFT)
            && IsActionUnlocked(InputAction::MOVE_LEFT)) {
            physComp->SetVelocity({ -mf_Speed * mf_dash_factor, physComp->GetVelocity().y });
            isWalking = true;
        }

        // ========================================================
        // ★追加：歩行時の砂煙エフェクト処理
        // ========================================================
        // 落下中やジャンプ中（Yの速度がある時）は空中にいるので砂煙を出さない
        if (isWalking && std::abs(physComp->GetVelocity().y) < 0.1f) {
            walkTimer++;

            // ダッシュ中は砂煙を多く出す（間隔を短くする）
            int dustInterval = (mf_dash_factor > 1.0f) ? 5 : 10;

            if (walkTimer % dustInterval == 0) {
                // 画像の中心から、画像の高さの半分(mn_ImageSize_y / 2)を足して足元のY座標を求める
                float footY = GetPosition().y + (mn_ImageSize_y / 2.0f);

                // 砂煙を発生
                particle.Play(GetPosition().x, footY, WalkDust);
            }
        }
        else {
            walkTimer = 0; // 止まったり空中にいる時はタイマーをリセット
        }
        // ========================================================


        // JUMP
        if (InputMapping::GetInstance().IsJustPressed(InputAction::JUMP)
            && IsActionUnlocked(InputAction::JUMP)) {
            bool onGround = false;
            Bounds2 temp_collider, player_collider; // 20260512 UPDATE
            player_collider = this->GetComponent<ColliderComponent>()->GetAbsoluteBounds();
            temp_collider.p_min = Vector2f(player_collider.p_min.x + 0.2f, player_collider.p_max.y);
            temp_collider.p_max = Vector2f(player_collider.p_min.x - 0.2f, player_collider.p_max.y + 0.2f);
            {
                auto colliders = GetScene()->FindObjectsWithTag("Tile");
                for (auto collider : colliders) {
                    if (collider->GetComponent<ColliderComponent>()->CheckCollision(GetComponent<ColliderComponent>())) {
                        // 20260512 UPDATE : 壁ジャンプ防止
                        if (collider->GetComponent<ColliderComponent>()->CheckCollision(temp_collider)) {
                            onGround = true;
                            break;
                        }
                    }
                }
            }

            {
                auto colliders = GetScene()->FindObjectsWithTag("MoveingPlatfrom");
                for (auto collider : colliders) {
                    if (collider->GetComponent<ColliderComponent>()->CheckCollision(GetComponent<ColliderComponent>())) {
                        onGround = true;
                        break;
                    }
                }
            }

            if (physComp->GetVelocity().y >= -0.1f && GetPosition().y >= 0.f
                && onGround) {
                m_Progress.inputCount++; // 20260508
                physComp->SetVelocity({ physComp->GetVelocity().x, mf_JumpForce });

                // ★おまけ：ジャンプした瞬間にも足元に砂煙を出すと気持ちいいです
                float footY = GetPosition().y + (mn_ImageSize_y / 2.0f);
                particle.Play(GetPosition().x, footY, WalkDust);
            }
        }

        // update player's position
        Vector2f displacement = physComp->GetDisplacement(deltaTime);
        GetTransform()->Translate(displacement.x, displacement.y);
    }

    // オブジェクトを拾う
    void TryPickUp() {
        // 拾うことが可能なオブジェクトを探す
        auto props = GetScene()->FindObjectsWithTag("Tile");

        for (auto* prop : props) {
            float dist = (prop->GetPosition() - this->GetPosition()).magnitude();

            if (dist < mf_InteractRange) {
                mp_HeldObject = prop;

                auto phys = mp_HeldObject->GetComponent<PhysicsComponent>();
                auto stateComp = mp_HeldObject->GetComponent<StateComponent>();
                if (phys && stateComp) {
                    phys->Reset();
                    phys->SetEnabled(false);
                    stateComp->SetState(ActorState::Held);
                }
                break;
            }
        }
    }

    // オブジェクトを捨てる
    void Throw() {
        if (!mp_HeldObject) return;

        auto stateComp = mp_HeldObject->GetComponent<StateComponent>();

        // コライダーコンポーネントを有効にする
        auto phys = mp_HeldObject->GetComponent<PhysicsComponent>();
        if (phys && stateComp) {
            stateComp->SetState(ActorState::Thrown);
            phys->SetEnabled(true);

            // 投げ出す力と方向を設定する
            float throwForce = 60.0f + (std::abs)(this->GetComponent<PhysicsComponent>()->GetVelocity().x);
            Vector2f throwDir = mb_FacingRight ? Vector2f(1.0f, -1.0f) : Vector2f(-1.0f, -1.0f);
            throwDir = throwDir.normalize() * throwForce;

            phys->SetVelocity(throwDir);
        }

        mp_HeldObject->SetCollidable(true);

        // 少しずらしてからポインターを削除する
        mp_HeldObject->GetTransform()->Translate(mb_FacingRight ? 10.f : -10.f, 0);
        mp_HeldObject = nullptr;
    }

    void Attack() {
        Vector2f offset = mb_FacingRight ? Vector2f(mf_AttackRange, 0) : Vector2f(-mf_AttackRange, 0);
        Vector2f attackCenter = GetPosition() + offset;

        // create attack collider
        Bounds2 attackBounds;
        attackBounds.p_min = attackCenter - Vector2f(mf_AttackRange, mf_AttackHeight);
        attackBounds.p_max = attackCenter + Vector2f(mf_AttackRange, mf_AttackHeight);
        attackCollider = attackBounds; // test

        auto scene = GetScene();
        if (!scene) return;

        const auto& targets = scene->GetObjects();
        for (const auto& target : targets) {
            if (target.get() == this) continue;

            auto col = target->GetCollider();
            if (col && col->GetAbsoluteBounds().Overlaps(attackBounds)
                && target->GetTag() == "Tile" && target->GetTag() != "MovingPlatform") {
                OnHitTarget(target.get());
            }
        }
    }

    void OnHitTarget(GameObject* target) {
        if (!target) return;

        float x = target->GetTransform()->mv_position.x;
        float y = target->GetTransform()->mv_position.y;
        auto state = target->GetComponent<StateComponent>();
        if (state) {
            state->SetState(ActorState::PendingDestroy);
        }

        effect.Play(x, y, Effect_Attack);
        target->Destroy();
        AudioManager::GetInstance()->PlaySound("Resource/sounds/SE/ヒット.mp3");
    }

    void DrawHPBar()
    {
        int barWidth = 500;
        int barHeight = 50;

        int x = 30;
        int y = 1000;

        // 背景（灰色）
        DrawBox(x, y, x + barWidth, y + barHeight, GetColor(80, 80, 80), TRUE);

        float HpRate = static_cast<float>(m_Progress.currentHp) / static_cast<float>(m_Progress.maxHp);
        int currentWidth = (int)(barWidth * HpRate);

        DrawBox(x, y, x + currentWidth, y + barHeight, GetColor(0, 255, 0), TRUE);
        DrawBox(x, y, x + barWidth, y + barHeight, GetColor(255, 255, 255), FALSE);
    }
};