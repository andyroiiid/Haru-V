//
// Created by Andrew Huang on 12/2/2022.
//

#include "physics/PhysicsLayer.h"

#include "physics/PhysicsUtilities.h"

//void PhysicsSetSimulationLayer(physx::PxRigidActor *actor, PhysicsLayer layer) {
//    const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(layer);
//    ForEachActorShape(actor, [&filterData](physx::PxShape *shape) {
//        shape->setSimulationFilterData(filterData);
//    });
//}

void PhysicsSetQueryLayer(physx::PxRigidActor *actor, PhysicsLayer layer) {
    const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(layer);
    PhysicsForEachActorShape(actor, [&filterData](physx::PxShape *shape) { shape->setQueryFilterData(filterData); });
}
