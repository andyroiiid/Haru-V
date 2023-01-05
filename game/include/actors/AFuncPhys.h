//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include "actors/Actor.h"
#include "map/Brushes.h"
#include "map/MapData.h"

namespace physx {
class PxRigidDynamic;
} // namespace physx

class AFuncPhys : public Actor {
public:
    DEFINE_ACTOR_CLASS(AFuncPhys)

    explicit AFuncPhys(const std::vector<MapData::Brush> &brushes);

    ~AFuncPhys() override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw() override;

private:
    Brushes                m_brushes;
    physx::PxRigidDynamic *m_rigidbody;

    glm::mat4 m_modelMatrix{1.0f};
};
