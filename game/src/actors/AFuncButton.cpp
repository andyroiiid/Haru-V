//
// Created by andyroiiid on 1/1/2023.
//

#include "actors/AFuncButton.h"

#include <script/LuaSandbox.h>

#include "Globals.h"

void AFuncButton::StartUse(APlayer *player, const physx::PxRaycastHit &hit) {
    Open();
    g_Lua->CallGlobalFunction(m_event, "press");
}

void AFuncButton::StopUse(APlayer *player) {
    Close();
    g_Lua->CallGlobalFunction(m_event, "release");

}
