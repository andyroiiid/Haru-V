//
// Created by andyroiiid on 12/28/2022.
//

#include "physics/PhysicsErrorCallback.h"

#include "core/Debug.h"

static void Info(const char *error, const char *message) {
    DebugInfo("PhysX {}: {}", error, message);
}

static void Warning(const char *error, const char *message) {
    DebugWarning("PhysX {}: {}", error, message);
}

static void Error(const char *error, const char *message) {
    DebugError("PhysX {}: {}", error, message);
}

void PhysicsErrorCallback::reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line) {
    const char *error                                   = nullptr;
    void (*loggingCallback)(const char *, const char *) = nullptr;
    switch (code) {
    case physx::PxErrorCode::eNO_ERROR:
        error           = "No Error";
        loggingCallback = Info;
        break;
    case physx::PxErrorCode::eDEBUG_INFO:
        error           = "Debug Info";
        loggingCallback = Info;
        break;
    case physx::PxErrorCode::eDEBUG_WARNING:
        error           = "Debug Warning";
        loggingCallback = Warning;
        break;
    case physx::PxErrorCode::eINVALID_PARAMETER:
        error           = "Invalid Parameter";
        loggingCallback = Error;
        break;
    case physx::PxErrorCode::eINVALID_OPERATION:
        error           = "Invalid Operation";
        loggingCallback = Error;
        break;
    case physx::PxErrorCode::eOUT_OF_MEMORY:
        error           = "Out of Memory";
        loggingCallback = Error;
        break;
    case physx::PxErrorCode::eINTERNAL_ERROR:
        error           = "Internal Error";
        loggingCallback = Error;
        break;
    case physx::PxErrorCode::eABORT:
        error           = "Abort";
        loggingCallback = Error;
        break;
    case physx::PxErrorCode::ePERF_WARNING:
        error           = "Performance Warning";
        loggingCallback = Warning;
        break;
    case physx::PxErrorCode::eMASK_ALL:
        error           = "Unknown Error";
        loggingCallback = Error;
        break;
    }
    loggingCallback(error, message);
}
