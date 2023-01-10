//
// Created by andyroiiid on 1/6/2023.
//

#include "actors/ATrigger.h"

#include "Globals.h"
#include "actors/APlayer.h"
#include "actors/APropPowerSphere.h"
#include "script/GameLua.h"

static bool CheckTargetType(Actor *actor, ATrigger::TargetType targetType) {
    switch (targetType) {
    case ATrigger::TARGET_TYPE_PLAYER:
        return actor->IsClass<APlayer>();
    case ATrigger::TARGET_TYPE_POWER_SPHERE:
        return actor->IsClass<APropPowerSphere>();
    }
    return false;
}

void ATrigger::OnTriggerEnter(Actor *other) {
    if (m_event.empty() || !CheckTargetType(other, m_targetType)) {
        return;
    }

    g_Lua->CallGlobalFunction(m_event, "enter");
}

void ATrigger::OnTriggerExit(Actor *other) {
    if (m_event.empty() || !CheckTargetType(other, m_targetType)) {
        return;
    }

    g_Lua->CallGlobalFunction(m_event, "exit");
}

void ATrigger::Draw() {
    if (g_ShowTriggers) {
        AFuncBrush::Draw();
    }
}
