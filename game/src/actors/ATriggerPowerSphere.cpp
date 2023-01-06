//
// Created by andyroiiid on 1/6/2023.
//

#include "actors/ATriggerPowerSphere.h"

#include <script/LuaSandbox.h>

#include "Globals.h"
#include "actors/APropPowerSphere.h"

void ATriggerPowerSphere::OnTriggerEnter(Actor *other) {
    if (m_event.empty() || !other->IsClass<APropPowerSphere>()) {
        return;
    }
    g_Lua->CallGlobalFunction(m_event, "enter");
}

void ATriggerPowerSphere::OnTriggerExit(Actor *other) {
    if (m_event.empty() || !other->IsClass<APropPowerSphere>()) {
        return;
    }
    g_Lua->CallGlobalFunction(m_event, "exit");
}
