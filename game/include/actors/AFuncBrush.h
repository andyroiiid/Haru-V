//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include "actors/Actor.h"
#include "map/Brushes.h"
#include "map/MapData.h"

namespace physx {
class PxRigidStatic;
} // namespace physx

class AFuncBrush : public Actor {
public:
    DEFINE_ACTOR_CLASS(AFuncBrush)

    explicit AFuncBrush(const std::vector<MapData::Brush> &brushes, BrushType type = BrushType::Normal, PhysicsLayer layer = PHYSICS_LAYER_0);

    ~AFuncBrush() override;

    void Update(float deltaTime) override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw() override;

    void Move(const glm::vec3 &deltaPosition);

private:
    Brushes               m_brushes;
    physx::PxRigidStatic *m_rigidbody;

    glm::mat4 m_translationMatrix{1.0f};
    glm::mat4 m_rotationMatrix{1.0f};

    glm::vec3 m_position{};
    glm::vec3 m_velocity{};
};
