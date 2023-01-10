//
// Created by andyroiiid on 1/1/2023.
//

#include "actors/AFuncButton.h"

#include "Globals.h"
#include "script/GameLua.h"

void AFuncButton::StartUse(Actor *user, const physx::PxRaycastHit &hit) {
    Open();
    g_Lua->CallGlobalFunction(m_event, "press");
}

void AFuncButton::StopUse(Actor *user) {
    Close();
    g_Lua->CallGlobalFunction(m_event, "release");
}
