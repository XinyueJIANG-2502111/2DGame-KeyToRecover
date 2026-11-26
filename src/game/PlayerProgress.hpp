#pragma once

#include "platform/input/InputAction.hpp"

#include <algorithm>
#include <vector>

struct PlayerProgress {
    int maxHp = 99;
    int currentHp = maxHp;
    int inputCount = 0;

    bool IsActionUnlocked(InputAction action) const {
        switch (action) {
        case InputAction::MOVE_RIGHT:
        case InputAction::PICKUP:
        case InputAction::MOVE_UP:
        case InputAction::PAUSE:
            return true;
        default:
            return HasCollected(action);
        }
    }

    bool HasCollected(InputAction action) const {
        return std::find(m_CollectedActions.begin(), m_CollectedActions.end(), action) != m_CollectedActions.end();
    }

    void CollectAction(InputAction action) {
        if (!HasCollected(action)) {
            m_CollectedActions.push_back(action);
        }
    }

    const std::vector<InputAction>& GetCollectedActions() const { return m_CollectedActions; }

private:
    std::vector<InputAction> m_CollectedActions;
};
