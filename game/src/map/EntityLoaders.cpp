//
// Created by andyroiiid on 12/29/2022.
//

#include "map/EntityLoaders.h"

#include <core/Debug.h>

#include "Globals.h"
#include "actors/Scene.h"
#include "actors/AFuncBrush.h"
#include "actors/APlayerNoClip.h"

void LoadWorldSpawn(const MapData::Entity &entity) {
    g_Scene->CreateActor<AFuncBrush>(entity.Brushes);
}

void LoadInfoPlayerStart(const MapData::Entity &entity) {
    glm::vec3 origin;
    DebugCheckCritical(entity.GetPropertyVector("origin", origin), "info_player_start doesn't have a valid origin!");

    g_Scene->CreateActor<APlayerNoClip>(origin);
}
