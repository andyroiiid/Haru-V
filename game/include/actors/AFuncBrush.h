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

class AFuncBrush final : public Actor {
public:
    DEFINE_ACTOR_CLASS(AFuncBrush)

    explicit AFuncBrush(const std::vector<MapData::Brush> &brushes);

    ~AFuncBrush() final;

    void Draw() final;

private:
    Brushes m_brushes;
    physx::PxRigidStatic *m_rigidbody;
};
