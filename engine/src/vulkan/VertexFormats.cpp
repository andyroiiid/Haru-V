//
// Created by andyroiiid on 12/21/2022.
//

#include "vulkan/VertexFormats.h"

#include <vector>
#include <vulkan/vulkan.hpp>

const vk::PipelineVertexInputStateCreateInfo *VertexPositionOnly::GetPipelineVertexInputStateCreateInfo() {
    static const std::vector<vk::VertexInputBindingDescription> bindings{
            {0, sizeof(VertexPositionOnly), vk::VertexInputRate::eVertex}
    };

    static const std::vector<vk::VertexInputAttributeDescription> attributes{
            {0, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(VertexPositionOnly, Position))},
    };

    static const vk::PipelineVertexInputStateCreateInfo vertexInput{{}, bindings, attributes};

    return &vertexInput;
}

const vk::PipelineVertexInputStateCreateInfo *VertexCanvas::GetPipelineVertexInputStateCreateInfo() {
    static const std::vector<vk::VertexInputBindingDescription> bindings{
            {0, sizeof(VertexCanvas), vk::VertexInputRate::eVertex}
    };

    static const std::vector<vk::VertexInputAttributeDescription> attributes{
            {0, 0, vk::Format::eR32G32Sfloat, static_cast<uint32_t>(offsetof(VertexCanvas, Position))},
            {1, 0, vk::Format::eR32G32Sfloat, static_cast<uint32_t>(offsetof(VertexCanvas, TexCoord))}
    };

    static const vk::PipelineVertexInputStateCreateInfo vertexInput{{}, bindings, attributes};

    return &vertexInput;
}

const vk::PipelineVertexInputStateCreateInfo *VertexBase::GetPipelineVertexInputStateCreateInfo() {
    static const std::vector<vk::VertexInputBindingDescription> bindings{
            {0, sizeof(VertexBase), vk::VertexInputRate::eVertex}
    };

    static const std::vector<vk::VertexInputAttributeDescription> attributes{
            {0, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(VertexBase, Position))},
            {1, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(VertexBase, Normal))},
            {2, 0, vk::Format::eR32G32Sfloat,    static_cast<uint32_t>(offsetof(VertexBase, TexCoord))}
    };

    static const vk::PipelineVertexInputStateCreateInfo vertexInput{{}, bindings, attributes};

    return &vertexInput;
}
