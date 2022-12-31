//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <glm/mat4x4.hpp>
#include <vulkan/VulkanBase.h>
#include <vulkan/TextureCache.h>
#include <vulkan/MeshCache.h>
#include <vulkan/VulkanUniformBufferSet.h>
#include <vulkan/VulkanPipeline.h>
#include <vulkan/VulkanMesh.h>
#include <vulkan/VertexFormats.h>

#include "gfx/ShadowContext.h"
#include "gfx/DeferredContext.h"
#include "gfx/PbrMaterialCache.h"

struct GLFWwindow;

struct alignas(256) RendererUniformData {
    glm::mat4 Projection;
    glm::mat4 View;
    glm::vec3 CameraPosition;
};

struct alignas(256) LightingUniformData {
    glm::vec3 LightDirection;
    [[maybe_unused]] float Padding0;
    glm::vec3 LightColor;
    [[maybe_unused]] float Padding1;
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

    VulkanMesh *LoadObjMesh(const std::string &objFilename) {
        return m_meshCache.LoadObjMesh(objFilename);
    }

    PbrMaterial *LoadPbrMaterial(const std::string &materialFilename) {
        return m_pbrMaterialCache.LoadMaterial(materialFilename);
    }

    void SetCameraData(const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPosition) {
        m_rendererUniformData.Projection = projection;
        m_rendererUniformData.View = view;
        m_rendererUniformData.CameraPosition = cameraPosition;
    }

    void SetLightingData(const glm::vec3 &lightDirection, const glm::vec3 &lightColor) {
        m_lightingUniformData.LightDirection = lightDirection;
        m_lightingUniformData.LightColor = lightColor;
    }

    void Draw(const VulkanMesh *mesh, const glm::mat4 &modelMatrix, const PbrMaterial *material) {
        m_drawCalls.emplace_back(mesh, modelMatrix, material);
    }

    void FinishDrawing();

private:
    void CreateUniformBuffers();

    void CreateIblTextureSet();

    void CreatePipelines();

    void CreateSkyboxCube();

    void CreateFullScreenQuad();

    void DrawToShadowMaps(vk::CommandBuffer cmd, uint32_t bufferingIndex);

    void DrawToDeferredTextures(vk::CommandBuffer cmd, uint32_t bufferingIndex);

    void DrawToScreen(const vk::RenderPassBeginInfo *primaryRenderPassBeginInfo, vk::CommandBuffer cmd, uint32_t bufferingIndex);

    VulkanBase m_device;
    TextureCache m_textureCache;
    PbrMaterialCache m_pbrMaterialCache;
    MeshCache m_meshCache;

    ShadowContext m_shadowContext;
    DeferredContext m_deferredContext;

    RendererUniformData m_rendererUniformData{};
    LightingUniformData m_lightingUniformData{};
    VulkanUniformBufferSet m_uniformBufferSet;

    vk::DescriptorSetLayout m_iblTextureSetLayout;
    vk::DescriptorSet m_iblTextureSet;

    VulkanPipeline m_basePipeline;
    VulkanPipeline m_skyboxPipeline;
    VulkanPipeline m_combinePipeline;

    VulkanMesh m_skyboxCube;
    VulkanMesh m_fullScreenQuad;

    struct DrawCall {
        const VulkanMesh *Mesh;
        glm::mat4 ModelMatrix;
        const PbrMaterial *Material;

        DrawCall(const VulkanMesh *mesh, const glm::mat4 &modelMatrix, const PbrMaterial *material)
                : Mesh(mesh),
                  ModelMatrix(modelMatrix),
                  Material(material) {}
    };

    std::vector<DrawCall> m_drawCalls;
};
