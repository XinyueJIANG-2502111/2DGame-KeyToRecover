#include "Scene.hpp"

#include "ScenePhysics.hpp"
#include "SceneRenderer.hpp"

#include <algorithm>
#include <utility>

void Scene::Update(double deltaTime) {
    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [](const auto& obj) { return !obj->IsActive(); }), objects.end());

    for (auto& obj : objects) {
        obj->Update(deltaTime);
    }

    UpdateScenePhysics(objects, m_WorldBounds);

    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [](const auto& obj) { return obj->IsPendingDestroy(); }), objects.end());
}

void Scene::Draw() {
    if (const auto* camera = GetCamera()) {
        DrawSceneObjects(objects, *camera);
    }
}

void Scene::SetWorldBounds(const Vector2f& min, const Vector2f& max) {
    m_WorldBounds = Bounds2(min, max);
}

void Scene::AddObject(std::unique_ptr<GameObject> obj) {
    obj->SetScene(this);
    objects.push_back(std::move(obj));
}

void Scene::RemoveAllObjects() {
    objects.clear();
}

GameObject* Scene::FindObjectWithTag(const std::string& tag) {
    auto it = std::find_if(objects.begin(), objects.end(), [&tag](const auto& obj) {
        return obj->IsActive() && obj->CompareTag(tag);
    });
    return it != objects.end() ? it->get() : nullptr;
}

std::vector<GameObject*> Scene::FindObjectsWithTag(const std::string& tag) {
    std::vector<GameObject*> result;
    for (const auto& obj : objects) {
        if (obj->IsActive() && obj->CompareTag(tag)) {
            result.push_back(obj.get());
        }
    }
    return result;
}
