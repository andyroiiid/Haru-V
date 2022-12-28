//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace vk {
    struct PipelineVertexInputStateCreateInfo;
}

struct VertexPositionOnly {
    glm::vec3 Position;

    VertexPositionOnly() = default;

    explicit VertexPositionOnly(const glm::vec3 &position)
            : Position(position) {}

    static const vk::PipelineVertexInputStateCreateInfo *GetPipelineVertexInputStateCreateInfo();
};

struct VertexCanvas {
    glm::vec2 Position;
    glm::vec2 TexCoord;

    VertexCanvas() = default;

    VertexCanvas(const glm::vec2 &position, const glm::vec2 &texCoord)
            : Position(position), TexCoord(texCoord) {}

    static const vk::PipelineVertexInputStateCreateInfo *GetPipelineVertexInputStateCreateInfo();
};

struct VertexBase {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;

    VertexBase() = default;

    VertexBase(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &texCoord)
            : Position(position), Normal(normal), TexCoord(texCoord) {}

    static const vk::PipelineVertexInputStateCreateInfo *GetPipelineVertexInputStateCreateInfo();
};
