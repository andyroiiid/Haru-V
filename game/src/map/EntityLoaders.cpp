//
// Created by andyroiiid on 12/29/2022.
//

#include "map/EntityLoaders.h"

#include <tracy/Tracy.hpp>
#include <core/Debug.h>

#include "Globals.h"
#include "actors/Scene.h"
#include "actors/AWorldSpawn.h"
#include "actors/APlayer.h"
#include "actors/AFuncBrush.h"
#include "actors/AFuncMove.h"
#include "actors/AFuncButton.h"
#include "actors/AFuncPhys.h"
#include "actors/APropTestModel.h"

void LoadWorldSpawn(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 lightDirection;
    DebugCheckCritical(entity.GetPropertyVector("light_direction", lightDirection), "worldspawn doesn't have a valid light_direction!");

    glm::vec3 lightColor;
    DebugCheckCritical(entity.GetPropertyColor("light_color", lightColor), "worldspawn doesn't have a valid light_color!");

    std::string script;
    DebugCheck(entity.GetPropertyString("script", script), "worldspawn doesn't have a valid script!");

    g_Scene->CreateActor<AWorldSpawn>(
            entity.Brushes,
            lightDirection,
            lightColor,
            script
    );
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

void LoadFuncButton(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 moveSpeed;
    DebugCheckCritical(entity.GetPropertyVector("move_speed", moveSpeed), "func_button doesn't have a valid move_speed!");

    float moveTime;
    DebugCheckCritical(entity.GetPropertyFloat("move_time", moveTime), "func_button doesn't have a valid move_time!");

    std::string event;
    DebugCheckCritical(entity.GetPropertyString("event", event), "func_button doesn't have a valid event!");

    g_Scene->CreateActor<AFuncButton>(entity.Brushes, moveSpeed, moveTime, event);
}

void LoadFuncPhys(const MapData::Entity &entity) {
    ZoneScoped;

    g_Scene->CreateActor<AFuncPhys>(entity.Brushes);
}

void LoadPropTestModel(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 origin;
    DebugCheckCritical(entity.GetPropertyVector("origin", origin), "prop_test_model doesn't have a valid origin!");

    std::string model;
    DebugCheckCritical(entity.GetPropertyString("model", model), "prop_test_model doesn't have a valid model!");

    g_Scene->CreateActor<APropTestModel>(
            "models/" + model + ".obj",
            "materials/" + model + ".json",
            origin
    );
}
