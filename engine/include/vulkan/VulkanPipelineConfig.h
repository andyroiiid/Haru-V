//
// Created by andyroiiid on 12/22/2022.
//

#pragma once

#include "vulkan/VulkanDevice.h"

struct VulkanPipelineConfig {
    std::string           VertexShader;
    std::string           GeometryShader;
    std::string           FragmentShader;
    VulkanPipelineOptions Options;

    explicit VulkanPipelineConfig(const std::string &jsonFilename);
};
