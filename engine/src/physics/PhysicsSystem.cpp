//
// Created by andyroiiid on 11/21/2022.
//

#include "physics/PhysicsSystem.h"

#include <vector>
#include <PxPhysicsAPI.h>

#include "core/Debug.h"
#include "physics/PhysicsErrorCallback.h"

static physx::PxDefaultAllocator s_Allocator;
static PhysicsErrorCallback s_ErrorCallback;

PhysicsSystem::PhysicsSystem() {
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, s_Allocator, s_ErrorCallback);
    DebugCheckCritical(m_foundation, "Failed to create PhysX foundation.");

    m_pvdTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    DebugCheckCritical(m_foundation, "Failed to create PhysX PVD transport.");

    m_pvd = PxCreatePvd(*m_foundation);
    DebugCheckCritical(m_foundation, "Failed to create PhysX PVD.");

    if (!m_pvd->connect(*m_pvdTransport, physx::PxPvdInstrumentationFlag::eALL)) {
        DebugWarning("Failed to connect to PVD.");
    }

    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), true, m_pvd);
    DebugCheckCritical(m_foundation, "Failed to create PhysX physics.");

    const physx::PxCookingParams cookingParams(m_physics->getTolerancesScale());
    m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, cookingParams);
    DebugCheckCritical(m_foundation, "Failed to create PhysX cooking.");

    DebugInfo("PhysX {}.{}.{}", PX_PHYSICS_VERSION_MAJOR, PX_PHYSICS_VERSION_MINOR, PX_PHYSICS_VERSION_BUGFIX);
}

PhysicsSystem::~PhysicsSystem() {
    PX_RELEASE(m_cooking);
    PX_RELEASE(m_physics)
    PX_RELEASE(m_pvd)
    PX_RELEASE(m_pvdTransport)
    PX_RELEASE(m_foundation)
}

physx::PxConvexMesh *PhysicsSystem::CreateConvexMesh(physx::PxU32 count, const physx::PxVec3 *vertices, physx::PxU16 vertexLimit) {
    physx::PxConvexMeshDesc desc;
    desc.points.count = count;
    desc.points.stride = sizeof(physx::PxVec3);
    desc.points.data = vertices;
    desc.flags =
            physx::PxConvexFlag::eCOMPUTE_CONVEX |
            physx::PxConvexFlag::eDISABLE_MESH_VALIDATION |
            physx::PxConvexFlag::eFAST_INERTIA_COMPUTATION;
    desc.vertexLimit = vertexLimit;

    physx::PxDefaultMemoryOutputStream buffer;
    if (!m_cooking->cookConvexMesh(desc, buffer)) {
        DebugError("Failed to create convex PhysX mesh.");
        return nullptr;
    }

    physx::PxDefaultMemoryInputData input(buffer.getData(), buffer.getSize());
    return m_physics->createConvexMesh(input);
}

physx::PxTriangleMesh *PhysicsSystem::CreateTriangleMesh(physx::PxU32 count, const physx::PxVec3 *vertices) {
    physx::PxTriangleMeshDesc desc;
    desc.points.count = count;
    desc.points.stride = sizeof(physx::PxVec3);
    desc.points.data = vertices;
    desc.triangles.count = count / 3;
    desc.triangles.stride = 3 * sizeof(physx::PxU32);
    std::vector<physx::PxU32> trianglesData(count);
    for (int i = 0; i < count; i++) {
        trianglesData[i] = i;
    }
    desc.triangles.data = trianglesData.data();

    physx::PxDefaultMemoryOutputStream buffer;
    if (!m_cooking->cookTriangleMesh(desc, buffer)) {
        DebugError("Failed to create triangle PhysX mesh.");
        return nullptr;
    }

    physx::PxDefaultMemoryInputData input(buffer.getData(), buffer.getSize());
    return m_physics->createTriangleMesh(input);
}
