//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include <foundation/PxErrorCallback.h>

class PhysicsErrorCallback : public physx::PxErrorCallback {
public:
    void reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line) override;
};
