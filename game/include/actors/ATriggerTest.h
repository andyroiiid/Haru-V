//
// Created by andyroiiid on 1/4/2023.
//

#pragma once

#include "actors/Actor.h"

namespace physx {
class PxShape;

class PxRigidStatic;
} // namespace physx

class ATriggerTest : public Actor {
public:
    DEFINE_ACTOR_CLASS(ATriggerTest)

    ATriggerTest();

    ~ATriggerTest() override;

private:
    physx::PxShape       *m_shape;
    physx::PxRigidStatic *m_rigidBody;
};
