//
// Created by andyroiiid on 12/29/2022.
//

#include "map/EntityLoaders.h"

#include <tracy/Tracy.hpp>
#include <core/Debug.h>

#include "Globals.h"
#include "actors/Scene.h"
#include "actors/AFuncBrush.h"
#include "actors/AFuncMove.h"
#include "actors/AFuncPhys.h"
#include "actors/APlayer.h"

void LoadWorldSpawn(const MapData::Entity &entity) {
    ZoneScoped;

    g_Scene->CreateActor<AFuncBrush>(entity.Brushes);
}

void LoadInfoPlayerStart(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 origin;
    DebugCheckCritical(entity.GetPropertyVector("origin", origin), "info_player_start doesn't have a valid origin!");

    g_Scene->CreateActor<APlayer>(origin);
}

void LoadFuncBrush(const MapData::Entity &entity) {
    ZoneScoped;

    g_Scene->CreateActor<AFuncBrush>(entity.Brushes);
}

void LoadFuncMove(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 moveSpeed;
    DebugCheckCritical(entity.GetPropertyVector("move_speed", moveSpeed), "func_move doesn't have a valid move_speed!");

    float moveTime;
    DebugCheckCritical(entity.GetPropertyFloat("move_time", moveTime), "func_move doesn't have a valid move_time!");

    g_Scene->CreateActor<AFuncMove>(entity.Brushes, moveSpeed, moveTime);
}

void LoadFuncPhys(const MapData::Entity &entity) {
    ZoneScoped;

    g_Scene->CreateActor<AFuncPhys>(entity.Brushes);
}
