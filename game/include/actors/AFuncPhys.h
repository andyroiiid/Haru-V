//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include "actors/Actor.h"
#include "map/MapData.h"
#include "map/Brushes.h"

namespace physx {
    class PxRigidDynamic;
}

class AFuncPhys : public Actor {
public:
    DEFINE_ACTOR_CLASS(AFuncPhys)

    explicit AFuncPhys(const std::vector<MapData::Brush> &brushes);

    ~AFuncPhys() override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw() override;

    void Use(APlayer *player, const physx::PxRaycastHit &hit) override;

private:
    Brushes m_brushes;
    physx::PxRigidDynamic *m_rigidbody;

    glm::mat4 m_modelMatrix{1.0f};
};
