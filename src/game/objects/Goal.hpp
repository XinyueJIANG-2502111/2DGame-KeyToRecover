#pragma once

#include "Tile.hpp"

class Goal : public Tile {
public:
    Goal(float posX, float posY, std::string filename) : Tile(posX, posY, filename) {
        SetTag("Goal");
    }

    void OnCollision(GameObject* other) override {
        if (other->CompareTag("Player")) {
            m_Reached = true;
        }
    }

    bool IsReached() const { return m_Reached; }

private:
    bool m_Reached = false;
};
