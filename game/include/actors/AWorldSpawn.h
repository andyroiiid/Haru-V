//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include "actors/Actor.h"
#include "map/MapData.h"
#include "map/Brushes.h"

namespace physx {
    class PxRigidStatic;
}

class AWorldSpawn final : public Actor {
public:
    DEFINE_ACTOR_CLASS(AWorldSpawn)

    explicit AWorldSpawn(const std::vector<MapData::Brush> &brushes);

    ~AWorldSpawn() final;

    void Draw() final;

private:
    Brushes m_brushes;
    physx::PxRigidStatic *m_rigidbody;
    glm::mat4 m_transform{1.0f};
};
