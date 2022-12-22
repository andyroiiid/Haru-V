//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <glm/mat4x4.hpp>

#include "vulkan/VulkanBase.h"
#include "vulkan/VulkanMesh.h"

#include "VertexFormats.h"

struct GLFWwindow;

struct alignas(256) RendererUniformData {
    glm::mat4 Projection;
    glm::mat4 View;
};

static_assert(sizeof(RendererUniformData) == 256);

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

    void SetCameraMatrices(const glm::mat4 &projection, const glm::mat4 &view) {
        m_rendererUniformData.Projection = projection;
        m_rendererUniformData.View = view;
    }

    void Draw(const VulkanMesh &mesh, const glm::mat4 &modelMatrix) {
        m_drawCalls.emplace_back(mesh, modelMatrix);
    }

    void DrawToScreen();

private:
    void CreateDescriptorSetLayout();

    void CreateBufferingObjects();

    void CreatePipeline();

    VulkanBase m_device;

    vk::DescriptorSetLayout m_rendererDescriptorSetLayout;

    RendererUniformData m_rendererUniformData{};
    VulkanBuffer m_rendererUniformBuffer;
    struct BufferingObjects {
        vk::DescriptorSet RendererDescriptorSet;
    };
    std::vector<BufferingObjects> m_bufferingObjects;

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
