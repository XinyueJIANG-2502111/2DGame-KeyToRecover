#pragma once

#include "game/ui/Menu.hpp"
#include "engine/scene/Scene.hpp"

class Player;
struct MapData;
struct PlayerProgress;

class Stage1 : public Scene {
public:
    explicit Stage1(PlayerProgress& progress) : m_Progress(progress) {}

    void Initialize() override;
    void Update(double deltaTime) override;
    void Draw() override;

private:
    void BuildStage(const MapData& mapData);

    PlayerProgress& m_Progress;
    Player* mp_Player = nullptr;
    Menu m_Menu;
};
