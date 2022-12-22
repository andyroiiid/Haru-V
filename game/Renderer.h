//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <glm/mat4x4.hpp>

#include "vulkan/VulkanBase.h"
#include "vulkan/VulkanMesh.h"

#include "VertexFormats.h"

struct GLFWwindow;

class Renderer {
public:
    explicit Renderer(GLFWwindow *window);

    ~Renderer();

    Renderer(const Renderer &) = delete;

    Renderer &operator=(const Renderer &) = delete;

    Renderer(Renderer &&) = delete;

    Renderer &operator=(Renderer &&) = delete;

    float GetAspectRatio() {
        const vk::Extent2D &extent = m_device.GetSwapchainExtent();
        return static_cast<float>(extent.width) / static_cast<float>(extent.height);
    }

    void WaitDeviceIdle() {
        m_device.WaitIdle();
    }

    VulkanMesh CreateMesh(const std::vector<VertexBase> &vertices) {
        return {m_device, vertices.size(), sizeof(VertexBase), vertices.data()};
    }

    void Draw(const VulkanMesh &mesh, const glm::mat4 &modelMatrix) {
        m_drawCalls.emplace_back(mesh, modelMatrix);
    }

    void DrawToScreen();

private:
    void CreatePipeline();

    VulkanBase m_device;

    vk::PipelineLayout m_pipelineLayout;
    vk::ShaderModule m_vertexShaderModule;
    vk::ShaderModule m_fragmentShaderModule;
    vk::Pipeline m_pipeline;

    struct DrawCall {
        const VulkanMesh *Mesh;
        glm::mat4 ModelMatrix;

        DrawCall(const VulkanMesh &mesh, const glm::mat4 &modelMatrix)
                : Mesh(&mesh),
                  ModelMatrix(modelMatrix) {}
    };

    std::vector<DrawCall> m_drawCalls;
};
