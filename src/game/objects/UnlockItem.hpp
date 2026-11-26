#pragma once
#include "Tile.hpp"
#include "platform/input/InputMapping.hpp"

class UnlockItem : public Tile {
private:
    InputAction m_TargetAction; // このアイテムがどのアクションを解放するか

public:
    // コンストラクタで「何を解放するアイテムか」を指定して生成する
    UnlockItem(float x, float y, InputAction action,std::string filename)
        : Tile(x,y, filename)      // 親クラス (GameObject) の座標初期化を呼ぶ
        , m_TargetAction(action) {
		SetTag("UnlockItem"); // タグを設定（必要に応じて）
        GameObject::SetPosition(Vector2f(x,y));
    }

    InputAction GetInputAction() { return m_TargetAction; }
};