//
// Created by andyroiiid on 12/22/2022.
//

#include "vulkan/VulkanPipelineConfig.h"

#include "core/Debug.h"
#include "file/JsonFile.h"

static vk::PrimitiveTopology TopologyFromString(const std::string &topology) {
    if (topology == "point_list") {
        return vk::PrimitiveTopology::ePointList;
    } else if (topology == "line_list") {
        return vk::PrimitiveTopology::eLineList;
    } else if (topology == "line_strip") {
        return vk::PrimitiveTopology::eLineStrip;
    } else if (topology == "triangle_list") {
        return vk::PrimitiveTopology::eTriangleList;
    } else if (topology == "triangle_strip") {
        return vk::PrimitiveTopology::eTriangleStrip;
    } else if (topology == "triangle_fan") {
        return vk::PrimitiveTopology::eTriangleFan;
    }
    DebugError("Invalid topology: {}", topology);
    exit(EXIT_FAILURE);
}

static vk::PolygonMode PolygonModeFromString(const std::string &polygonMode) {
    if (polygonMode == "fill") {
        return vk::PolygonMode::eFill;
    } else if (polygonMode == "line") {
        return vk::PolygonMode::eLine;
    } else if (polygonMode == "point") {
        return vk::PolygonMode::ePoint;
    }
    DebugError("Invalid polygon mode: {}", polygonMode);
    exit(EXIT_FAILURE);
}

static vk::CullModeFlags CullModeFromString(const std::string &cullMode) {
    if (cullMode == "none") {
        return vk::CullModeFlagBits::eNone;
    } else if (cullMode == "back") {
        return vk::CullModeFlagBits::eBack;
    } else if (cullMode == "front") {
        return vk::CullModeFlagBits::eFront;
    } else if (cullMode == "front_and_back") {
        return vk::CullModeFlagBits::eFrontAndBack;
    }
    DebugError("Invalid cull mode: {}", cullMode);
    exit(EXIT_FAILURE);
}

static vk::CompareOp CompareOpFromString(const std::string &compareOp) {
    if (compareOp == "never") {
        return vk::CompareOp::eNever;
    } else if (compareOp == "less") {
        return vk::CompareOp::eLess;
    } else if (compareOp == "equal") {
        return vk::CompareOp::eEqual;
    } else if (compareOp == "less_or_equal") {
        return vk::CompareOp::eLessOrEqual;
    } else if (compareOp == "greater") {
        return vk::CompareOp::eGreater;
    } else if (compareOp == "not_equal") {
        return vk::CompareOp::eNotEqual;
    } else if (compareOp == "greater_or_equal") {
        return vk::CompareOp::eGreaterOrEqual;
    } else if (compareOp == "always") {
        return vk::CompareOp::eAlways;
    }
    DebugError("Invalid compare op: {}", compareOp);
    exit(EXIT_FAILURE);
}

VulkanPipelineConfig::VulkanPipelineConfig(const std::string &jsonFilename) {
    JsonFile pipelineJson(jsonFilename);
    VertexShader                  = pipelineJson.GetString("vertex");
    GeometryShader                = pipelineJson.GetString("geometry", {});
    FragmentShader                = pipelineJson.GetString("fragment");
    const std::string topology    = pipelineJson.GetString("topology", {});
    Options.Topology              = topology.empty() ? vk::PrimitiveTopology::eTriangleList : TopologyFromString(topology);
    const std::string polygonMode = pipelineJson.GetString("polygon_mode", {});
    Options.PolygonMode           = polygonMode.empty() ? vk::PolygonMode::eFill : PolygonModeFromString(polygonMode);
    const std::string cullMode    = pipelineJson.GetString("cull_mode", {});
    Options.CullMode              = cullMode.empty() ? vk::CullModeFlagBits::eBack : CullModeFromString(cullMode);
    Options.DepthTestEnable       = pipelineJson.GetBoolean("depth_test", true) ? VK_TRUE : VK_FALSE;
    Options.DepthWriteEnable      = pipelineJson.GetBoolean("depth_write", true) ? VK_TRUE : VK_FALSE;
    const std::string compareOp   = pipelineJson.GetString("compare_op", {});
    Options.DepthCompareOp        = compareOp.empty() ? vk::CompareOp::eLess : CompareOpFromString(compareOp);
}
