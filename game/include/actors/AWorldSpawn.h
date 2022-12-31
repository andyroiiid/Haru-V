//
// Created by andyroiiid on 12/31/2022.
//

#pragma once

#include <numeric>

#include "actors/AFuncBrush.h"

class AWorldSpawn : public AFuncBrush {
public:
    DEFINE_ACTOR_CLASS(AWorldSpawn)

    explicit AWorldSpawn(const std::vector<MapData::Brush> &brushes);

    void Draw() override;

private:
    glm::vec3 m_geomMin{std::numeric_limits<float>::max()};
    glm::vec3 m_geomMax{std::numeric_limits<float>::min()};
};
