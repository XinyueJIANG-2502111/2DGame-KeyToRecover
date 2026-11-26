#include "ScenePhysics.hpp"

#include "engine/objects/GameObject.hpp"
#include "engine/components/PhysicsComponent.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <unordered_set>

namespace {
    constexpr int kSolverIterations = 4;

    bool CanContact(const GameObject& object) {
        return object.IsCollidable() && !object.IsPendingDestroy() && object.GetCollider();
    }

    void ResolveCollision(GameObject& a, GameObject& b) {
        auto* physA = a.GetComponent<PhysicsComponent>();
        auto* physB = b.GetComponent<PhysicsComponent>();
        if (!physA || !physB) return;

        const float invMassA = physA->GetInvMass();
        const float invMassB = physB->GetInvMass();
        const float totalInvMass = invMassA + invMassB;
        if (totalInvMass == 0.f) return;

        const Bounds2 boundsA = a.GetCollider()->GetAbsoluteBounds();
        const Bounds2 boundsB = b.GetCollider()->GetAbsoluteBounds();
        if (!boundsA.Overlaps(boundsB)) return;

        const float overlapLeft = boundsA.p_max.x - boundsB.p_min.x;
        const float overlapRight = boundsB.p_max.x - boundsA.p_min.x;
        const float overlapTop = boundsA.p_max.y - boundsB.p_min.y;
        const float overlapBottom = boundsB.p_max.y - boundsA.p_min.y;
        const float minOverlapX = (std::min)(overlapLeft, overlapRight);
        const float minOverlapY = (std::min)(overlapTop, overlapBottom);
        const float massRatioA = invMassA / totalInvMass;
        const float massRatioB = invMassB / totalInvMass;

        if (minOverlapX < minOverlapY) {
            constexpr float edgeTolerance = 0.8f;
            if (minOverlapX < edgeTolerance
                && (std::abs(physA->GetVelocity().y) > 0.01f || std::abs(physB->GetVelocity().y) > 0.01f)) {
                return;
            }

            const float move = overlapLeft < overlapRight ? -minOverlapX : minOverlapX;
            a.GetTransform()->Translate(move * massRatioA, 0.f);
            b.GetTransform()->Translate(-move * massRatioB, 0.f);
            physA->SetVelocity({ 0.f, physA->GetVelocity().y });
            physB->SetVelocity({ 0.f, physB->GetVelocity().y });
        } else {
            const float move = overlapTop < overlapBottom ? -minOverlapY : minOverlapY;
            a.GetTransform()->Translate(0.f, move * massRatioA);
            b.GetTransform()->Translate(0.f, -move * massRatioB);
            physA->SetVelocity({ physA->GetVelocity().x, 0.f });
            physB->SetVelocity({ physB->GetVelocity().x, 0.f });
        }
    }

    void ResolveWorldBounds(const std::vector<GameObject*>& colliders, const Bounds2& worldBounds) {
        for (auto* object : colliders) {
            if (!CanContact(*object) || !object->GetComponent<PhysicsComponent>()) continue;

            const Bounds2 bounds = object->GetCollider()->GetAbsoluteBounds();
            Vector2f correction(0.f, 0.f);
            if (bounds.p_min.x < worldBounds.p_min.x) {
                correction.x = worldBounds.p_min.x - bounds.p_min.x;
            } else if (bounds.p_max.x > worldBounds.p_max.x) {
                correction.x = worldBounds.p_max.x - bounds.p_max.x;
            }

            if (bounds.p_min.y < worldBounds.p_min.y) {
                correction.y = worldBounds.p_min.y - bounds.p_min.y;
            } else if (bounds.p_max.y > worldBounds.p_max.y) {
                correction.y = worldBounds.p_max.y - bounds.p_max.y;
            }

            if (correction.x != 0.f || correction.y != 0.f) {
                object->GetTransform()->Translate(correction.x, correction.y);
            }
        }
    }
}

void UpdateScenePhysics(const std::vector<std::unique_ptr<GameObject>>& objects, const Bounds2& worldBounds) {
    std::vector<GameObject*> colliders;
    colliders.reserve(objects.size());
    for (const auto& object : objects) {
        if (object->GetCollider()) {
            colliders.push_back(object.get());
        }
    }

    // Contacts discovered in later solver passes still notify both objects only once.
    std::unordered_set<std::size_t> notifiedPairs;
    for (int iteration = 0; iteration < kSolverIterations; ++iteration) {
        for (std::size_t i = 0; i < colliders.size(); ++i) {
            auto& a = *colliders[i];
            for (std::size_t j = i + 1; j < colliders.size(); ++j) {
                auto& b = *colliders[j];
                if (!CanContact(a) || !CanContact(b) || !a.GetCollider()->CheckCollision(b.GetCollider())) continue;

                if (notifiedPairs.insert(i * colliders.size() + j).second) {
                    a.OnCollision(&b);
                    b.OnCollision(&a);
                }
                if (CanContact(a) && CanContact(b)) {
                    ResolveCollision(a, b);
                }
            }
        }
        ResolveWorldBounds(colliders, worldBounds);
    }
}
