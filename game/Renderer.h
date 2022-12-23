//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <glm/mat4x4.hpp>

#include "vulkan/VulkanBase.h"
#include "vulkan/VulkanUniformBufferSet.h"
#include "vulkan/VulkanPipeline.h"
#include "vulkan/VulkanMesh.h"
#include "vulkan/VulkanTexture.h"

#include "VertexFormats.h"

struct GLFWwindow;

struct alignas(256) RendererUniformData {
    glm::mat4 Projection;
    glm::mat4 View;
};

struct alignas(256) LightingUniformData {
    glm::vec3 LightDirection;
    [[maybe_unused]] float Padding0;
    glm::vec3 LightColor;
    [[maybe_unused]] float Padding1;
    glm::vec3 AmbientColor;
    [[maybe_unused]] float Padding2;
};

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

    void SetLightingData(const glm::vec3 &lightDirection, const glm::vec3 &lightColor, const glm::vec3 &ambientColor) {
        m_lightingUniformData.LightDirection = lightDirection;
        m_lightingUniformData.LightColor = lightColor;
        m_lightingUniformData.AmbientColor = ambientColor;
    }

    void Draw(const VulkanMesh &mesh, const glm::mat4 &modelMatrix) {
        m_drawCalls.emplace_back(mesh, modelMatrix);
    }

    void DrawToScreen();

private:
    void CreateUniformBuffers();

    void CreateTextureSet();

    void CreatePipeline();

    VulkanBase m_device;

    RendererUniformData m_rendererUniformData{};
    LightingUniformData m_lightingUniformData{};
    VulkanUniformBufferSet m_uniformBufferSet;

    vk::DescriptorSetLayout m_textureSetLayout;
    VulkanTexture m_texture;
    vk::DescriptorSet m_textureSet;

    VulkanPipeline m_pipeline;

    struct DrawCall {
        const VulkanMesh *Mesh;
        glm::mat4 ModelMatrix;

        DrawCall(const VulkanMesh &mesh, const glm::mat4 &modelMatrix)
                : Mesh(&mesh),
                  ModelMatrix(modelMatrix) {}
    };

    std::vector<DrawCall> m_drawCalls;
};
