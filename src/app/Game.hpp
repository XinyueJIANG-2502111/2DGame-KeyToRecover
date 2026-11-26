#pragma once

#include "game/PlayerProgress.hpp"

#include <chrono>

class Game {
public:
    Game();
    ~Game();

    void Run();
    void UpdateLogic(double deltaTime);
    void UpdateRender(double interpolationAlpha);
    void Draw();

    bool IsRunning() const { return mb_IsRunning; }

private:
    bool mb_IsRunning;
    bool mb_initSuccess;
    std::chrono::steady_clock::time_point m_LastFrameTime;
    double md_Accumulator;
    PlayerProgress m_Progress;
};
