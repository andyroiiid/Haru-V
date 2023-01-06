//
// Created by andyroiiid on 1/6/2023.
//

#include "actors/ATriggerPlayer.h"

#include <script/LuaSandbox.h>

#include "Globals.h"
#include "actors/APlayer.h"

void ATriggerPlayer::OnTriggerEnter(Actor *other) {
    if (m_event.empty() || !other->IsClass<APlayer>()) {
        return;
    }
    g_Lua->CallGlobalFunction(m_event, "enter");
}

void ATriggerPlayer::OnTriggerExit(Actor *other) {
    if (m_event.empty() || !other->IsClass<APlayer>()) {
        return;
    }
    g_Lua->CallGlobalFunction(m_event, "exit");
}
