//
// Created by andyroiiid on 1/8/2023.
//

#pragma once

#include <PxPhysicsAPI.h>
#include <vector>

template<class Func>
static std::vector<physx::PxShape *> PhysicsForEachActorShape(physx::PxRigidActor *actor, Func &&func) {
    std::vector<physx::PxShape *> shapes(actor->getNbShapes());
    actor->getShapes(shapes.data(), shapes.size());
    for (physx::PxShape *shape: shapes) {
        func(shape);
    }
    return shapes;
}

static inline void PhysicsSetActorMaterial(physx::PxRigidActor *actor, physx::PxMaterial *material) {
    PhysicsForEachActorShape(actor, [material](physx::PxShape *shape) { shape->setMaterials(&material, 1); });
}
