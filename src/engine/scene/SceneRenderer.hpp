#pragma once

#include "engine/objects/GameObject.hpp"

#include <algorithm>
#include <memory>
#include <vector>

inline void DrawSceneObjects(const std::vector<std::unique_ptr<GameObject>>& objects, const Camera& camera) {
    std::vector<GameObject*> drawOrder;
    drawOrder.reserve(objects.size());
    for (const auto& object : objects) {
        drawOrder.push_back(object.get());
    }

    std::stable_sort(drawOrder.begin(), drawOrder.end(), [](const auto* a, const auto* b) {
        return a->GetZOrder() < b->GetZOrder();
    });
    for (auto* object : drawOrder) {
        object->Draw(camera);
    }
}
