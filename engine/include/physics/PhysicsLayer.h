//
// Created by Andrew Huang on 12/2/2022.
//

#pragma once

#include <PxFiltering.h>
#include <foundation/PxSimpleTypes.h>

namespace physx {
    class PxRigidActor;
}

enum PhysicsLayer : physx::PxU32 {
    PHYSICS_LAYER_0 = (1u << 0),    // world default
    PHYSICS_LAYER_1 = (1u << 1),    // character controller default
    PHYSICS_LAYER_2 = (1u << 2),
    PHYSICS_LAYER_3 = (1u << 3),
    PHYSICS_LAYER_4 = (1u << 4),
    PHYSICS_LAYER_5 = (1u << 5),
    PHYSICS_LAYER_6 = (1u << 6),
    PHYSICS_LAYER_7 = (1u << 7),
    PHYSICS_LAYER_8 = (1u << 8),
    PHYSICS_LAYER_9 = (1u << 9),
    PHYSICS_LAYER_10 = (1u << 10),
    PHYSICS_LAYER_11 = (1u << 11),
    PHYSICS_LAYER_12 = (1u << 12),
    PHYSICS_LAYER_13 = (1u << 13),
    PHYSICS_LAYER_14 = (1u << 14),
    PHYSICS_LAYER_15 = (1u << 15),
    PHYSICS_LAYER_16 = (1u << 16),
    PHYSICS_LAYER_17 = (1u << 17),
    PHYSICS_LAYER_18 = (1u << 18),
    PHYSICS_LAYER_19 = (1u << 19),
    PHYSICS_LAYER_20 = (1u << 20),
    PHYSICS_LAYER_21 = (1u << 21),
    PHYSICS_LAYER_22 = (1u << 22),
    PHYSICS_LAYER_23 = (1u << 23),
    PHYSICS_LAYER_24 = (1u << 24),
    PHYSICS_LAYER_25 = (1u << 25),
    PHYSICS_LAYER_26 = (1u << 26),
    PHYSICS_LAYER_27 = (1u << 27),
    PHYSICS_LAYER_28 = (1u << 28),
    PHYSICS_LAYER_29 = (1u << 29),
    PHYSICS_LAYER_30 = (1u << 30),
    PHYSICS_LAYER_31 = (1u << 31)
};

static inline physx::PxFilterData PhysicsFilterDataFromLayer(PhysicsLayer layer) {
    return {layer, 0, 0, 0};
}

void PhysicsSetQueryLayer(physx::PxRigidActor *actor, PhysicsLayer layer);
