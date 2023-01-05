//
// Created by andyroiiid on 1/4/2023.
//
#include "actors/ATriggerTest.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>

#include "Globals.h"

ATriggerTest::ATriggerTest() {
    m_shape = g_PhysicsScene->CreateShape(physx::PxBoxGeometry(2.0f, 2.0f, 2.0f), true);
    m_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
    m_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);

    m_rigidBody = g_PhysicsScene->CreateStatic(physx::PxTransform{0.0f, 0.0f, 0.0f});
    m_rigidBody->attachShape(*m_shape);

    m_rigidBody->userData = this;
}

ATriggerTest::~ATriggerTest() {
    PX_RELEASE(m_rigidBody)

    PX_RELEASE(m_shape)
}
