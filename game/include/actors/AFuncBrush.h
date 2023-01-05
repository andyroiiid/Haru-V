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

    explicit AFuncBrush(const std::vector<MapData::Brush> &brushes);

    ~AFuncBrush() override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw() override;

    void Move(const glm::vec3 &deltaPosition);

private:
    Brushes               m_brushes;
    physx::PxRigidStatic *m_rigidbody;

    glm::mat4 m_modelMatrix{1.0f};
};
