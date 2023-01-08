//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include <foundation/Px.h>

namespace physx {
class PxFoundation;

class PxPvdTransport;

class PxPvd;

class PxPhysics;

class PxCooking;

class PxMaterial;

class PxConvexMesh;

class PxTriangleMesh;
} // namespace physx

class PhysicsSystem {
public:
    PhysicsSystem();

    ~PhysicsSystem();

    PhysicsSystem(const PhysicsSystem &) = delete;

    PhysicsSystem &operator=(const PhysicsSystem &) = delete;

    PhysicsSystem(PhysicsSystem &&) = delete;

    PhysicsSystem &operator=(PhysicsSystem &&) = delete;

    physx::PxMaterial *CreateMaterial(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution);

    physx::PxConvexMesh *CreateConvexMesh(
        physx::PxU32         count,
        const physx::PxVec3 *vertices,
        // The number of vertices and faces of a convex mesh in PhysX is limited to 255.
        physx::PxU16         vertexLimit = 255
    );

    physx::PxTriangleMesh *CreateTriangleMesh(physx::PxU32 count, const physx::PxVec3 *vertices);

private:
    friend class PhysicsScene;

    physx::PxFoundation   *m_foundation   = nullptr;
    physx::PxPvdTransport *m_pvdTransport = nullptr;
    physx::PxPvd          *m_pvd          = nullptr;
    physx::PxPhysics      *m_physics      = nullptr;
    physx::PxCooking      *m_cooking      = nullptr;
};
