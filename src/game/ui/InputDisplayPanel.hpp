#pragma once

#include "engine/objects/GameObject.hpp"
#include "platform/dxlib/GraphicsManager.hpp"
#include "game/PlayerProgress.hpp"
#include "platform/dxlib/ResourceManager.hpp"

class InputDisplayPanel : public GameObject {
public:
    InputDisplayPanel(float x, float y, const std::string& filename, const PlayerProgress& progress)
        : m_PanelPath(filename), m_Progress(progress) {
        SetPosition(Vector2f(x, y));
        SetTag("DisplayPanel");

        auto& resources = ResourceManager::GetInstance();
        const int panelHandle = resources.LoadGraphic(m_PanelPath);
        int panelWidth = 0;
        GraphicsManager::GetInstance().GetGraphSize(panelHandle, &panelWidth, &m_PanelHeight);
        resources.LoadGraphic(kRightPath);
        resources.LoadGraphic(kMenuPath);
        for (const auto& icon : kActionIcons) {
            resources.LoadGraphic(icon.path);
        }
    }

    ~InputDisplayPanel() override {
        auto& resources = ResourceManager::GetInstance();
        resources.ReleaseGraphic(m_PanelPath);
        resources.ReleaseGraphic(kRightPath);
        resources.ReleaseGraphic(kMenuPath);
        for (const auto& icon : kActionIcons) {
            resources.ReleaseGraphic(icon.path);
        }
    }

    void Draw(const Camera&) override {
        if (!IsActive()) return;

        auto& resources = ResourceManager::GetInstance();
        auto& graphics = GraphicsManager::GetInstance();
        const int panelX = static_cast<int>(GetPosition().x - 310.f);
        const int panelY = static_cast<int>(GetPosition().y - 200.f);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        graphics.DrawGraph(panelX, panelY, resources.GetGraphicHandle(m_PanelPath), true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        int offsetX = 0;
        int offsetY = 0;
        const auto drawIcon = [&](const char* path) {
            graphics.DrawGraph(panelX + 30 + offsetX, panelY + 40 + offsetY,
                resources.GetGraphicHandle(path), true);
            offsetY += 80;
            if (offsetY > m_PanelHeight - 80) {
                offsetY = 0;
                offsetX += 150;
            }
        };

        drawIcon(kRightPath);
        drawIcon(kMenuPath);
        for (InputAction action : m_Progress.GetCollectedActions()) {
            for (const auto& icon : kActionIcons) {
                if (icon.action == action) {
                    drawIcon(icon.path);
                    break;
                }
            }
        }
    }

private:
    struct ActionIcon {
        InputAction action;
        const char* path;
    };

    inline static constexpr const char* kRightPath = "Resource/images/右.png";
    inline static constexpr const char* kMenuPath = "Resource/images/メニュー.png";
    inline static constexpr ActionIcon kActionIcons[] = {
        { InputAction::MOVE_LEFT, "Resource/images/左.png" },
        { InputAction::JUMP, "Resource/images/ジャンプ.png" },
        { InputAction::ATTACK, "Resource/images/攻撃.png" },
        { InputAction::PICKUP, "Resource/images/掴む.png" },
    };

    std::string m_PanelPath;
    const PlayerProgress& m_Progress;
    int m_PanelHeight = 0;
};
