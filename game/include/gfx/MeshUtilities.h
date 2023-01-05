//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <vector>
#include <vulkan/VertexFormats.h>

std::vector<VertexPositionOnly> CreateSkyboxVertices();

void AppendRectVertices(
    std::vector<VertexCanvas> &vertices,
    const glm::vec2           &minPosition,
    const glm::vec2           &maxPosition,
    const glm::vec2           &minTexCoord,
    const glm::vec2           &maxTexCoord
);

void AppendBoxVertices(std::vector<VertexBase> &vertices, const glm::vec3 &min, const glm::vec3 &max);
