//
// Created by andyroiiid on 1/1/2023.
//

#include "actors/AFuncButton.h"

#include <script/LuaSandbox.h>

#include "Globals.h"

void AFuncButton::FixedUpdate(float fixedDeltaTime) {
    AFuncMove::FixedUpdate(fixedDeltaTime);

    if (IsOpen()) {
        Close();
    }
}

void AFuncButton::Use(APlayer *player, const physx::PxRaycastHit &hit) {
    if (!IsClosed()) {
        return;
    }

    Open();

    g_Lua->CallGlobalFunction(m_event);
}
