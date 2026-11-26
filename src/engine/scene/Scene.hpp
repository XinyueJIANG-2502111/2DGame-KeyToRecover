#pragma once

#include "engine/math/Bounds2.hpp"
#include "Camera.hpp"
#include "engine/objects/GameObject.hpp"

#include <memory>
#include <vector>

class Scene {
public:
    virtual ~Scene() = default;

    virtual void Initialize() {}
    virtual void Update(double deltaTime);
    virtual void Draw();
    virtual void Finalize() {}

    virtual Camera* GetCamera() { return m_Camera.get(); }
    void SetWorldBounds(const Vector2f& min, const Vector2f& max);

    void AddObject(std::unique_ptr<GameObject> obj);
    void RemoveAllObjects();
    const std::vector<std::unique_ptr<GameObject>>& GetObjects() const { return objects; }
    GameObject* FindObjectWithTag(const std::string& tag);
    std::vector<GameObject*> FindObjectsWithTag(const std::string& tag);

protected:
    std::unique_ptr<Camera> m_Camera;

private:
    std::vector<std::unique_ptr<GameObject>> objects;
    Bounds2 m_WorldBounds{ Vector2f(0, 0), Vector2f(2000.f, 2000.f) };
};
