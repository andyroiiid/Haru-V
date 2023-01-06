//
// Created by andyroiiid on 12/29/2022.
//

#pragma once

#include "map/MapData.h"

void LoadWorldSpawn(const MapData::Entity &entity);

void LoadInfoPlayerStart(const MapData::Entity &entity);

void LoadFuncBrush(const MapData::Entity &entity);

void LoadFuncMove(const MapData::Entity &entity);

void LoadFuncButton(const MapData::Entity &entity);

void LoadFuncPhys(const MapData::Entity &entity);

void LoadTriggerPlayer(const MapData::Entity &entity);

void LoadTriggerPowerSphere(const MapData::Entity &entity);

void LoadLightPoint(const MapData::Entity &entity);

void LoadPropTestModel(const MapData::Entity &entity);

void LoadPropPowerSphere(const MapData::Entity &entity);
