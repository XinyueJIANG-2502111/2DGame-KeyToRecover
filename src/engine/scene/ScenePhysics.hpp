#pragma once

#include <memory>
#include <vector>

class Bounds2;
class GameObject;

void UpdateScenePhysics(const std::vector<std::unique_ptr<GameObject>>& objects, const Bounds2& worldBounds);
