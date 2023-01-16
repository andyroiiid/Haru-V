//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <glm/mat4x4.hpp>
#include <math/ShadowMatrixCalculator.h>
#include <vulkan/MeshCache.h>
#include <vulkan/TextureCache.h>
#include <vulkan/VertexFormats.h>
#include <vulkan/VulkanBase.h>
#include <vulkan/VulkanMesh.h>
#include <vulkan/VulkanPipeline.h>
#include <vulkan/VulkanUniformBufferSet.h>

#include "gfx/DeferredContext.h"
#include "gfx/PbrMaterialCache.h"
#include "gfx/PostProcessingContext.h"
#include "gfx/ShadowContext.h"
#include "gfx/SingleTextureMaterialCache.h"

struct GLFWwindow;

struct alignas(256) RendererUniformData {
    glm::mat4              Projection;
    glm::mat4              View;
    glm::vec3              CameraPosition;
    [[maybe_unused]] float Padding;
    glm::vec4              ScreenInfo;
    glm::vec4              ScaledScreenInfo;
};

struct alignas(16) PointLightData {
    glm::vec3              Position;
    [[maybe_unused]] float Radius;
    glm::vec3              Color;
    [[maybe_unused]] float Padding;

    PointLightData() = default;

    PointLightData(const glm::vec3 &position, float radius, const glm::vec3 &color)
        : Position(position)
        , Radius(radius)
        , Color(color)
        , Padding(0.0f) {}
};

struct alignas(256) LightingUniformData {
    glm::vec3              LightDirection;
    int32_t                NumPointLights;
    glm::vec3              LightColor;
    [[maybe_unused]] float Padding0;
    glm::vec3              CascadeShadowMapSplits;
    [[maybe_unused]] float Padding1;
    glm::mat4              ShadowMatrices[4];
    PointLightData         PointLights[128];
};

class PbrRenderer {
public:
    explicit PbrRenderer(GLFWwindow *window);

    ~PbrRenderer();

    PbrRenderer(const PbrRenderer &) = delete;

    PbrRenderer &operator=(const PbrRenderer &) = delete;

    PbrRenderer(PbrRenderer &&) = delete;

    PbrRenderer &operator=(PbrRenderer &&) = delete;

    [[nodiscard]] glm::vec2 GetScreenExtent() const {
        const vk::Extent2D &swapchainExtent = m_device.GetSwapchainExtent();
        return {swapchainExtent.width, swapchainExtent.height};
    }

    void WaitDeviceIdle() { m_device.WaitIdle(); }

    VulkanMesh CreateMesh(const std::vector<VertexBase> &vertices) { return {m_device, vertices.size(), sizeof(VertexBase), vertices.data()}; }

    VulkanMesh *LoadObjMesh(const std::string &objFilename) { return m_meshCache.LoadObjMesh(objFilename); }

    PbrMaterial *LoadPbrMaterial(const std::string &materialFilename) { return m_pbrMaterialCache.LoadMaterial(materialFilename); }

    SingleTextureMaterial *LoadSingleTextureMaterial(const std::string &materialFilename) {
        return m_singleTextureMaterialCache.LoadMaterial(materialFilename);
    }

    void SetCameraData(const glm::vec3 &cameraPosition, const glm::mat4 &view, float fov, float near, float far);

    void SetLightingData(const glm::vec3 &lightDirection, const glm::vec3 &lightColor);

    void SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max);

    void DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, float radius) { m_pointLights.emplace_back(position, radius, color); }

    void Draw(const VulkanMesh *mesh, const glm::mat4 &modelMatrix, const PbrMaterial *material) {
        if (material->Transparent) {
            m_forwardDrawCalls.emplace_back(mesh, modelMatrix, material);
        } else {
            m_deferredDrawCalls.emplace_back(mesh, modelMatrix, material);
        }
    }

    void DrawScreenRect(
        const glm::vec2       &pMin,
        const glm::vec2       &pMax,
        const glm::vec2       &uvMin,
        const glm::vec2       &uvMax,
        const glm::vec4       &color,
        SingleTextureMaterial *texture
    ) {
        m_screenRectDrawCalls.emplace_back(pMin, pMax, uvMin, uvMax, color, texture);
    }

    void DrawScreenLine(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec4 &color) { m_screenLineDrawCalls.emplace_back(p0, p1, color); }

    void FinishDrawing();

private:
    void CreateUniformBuffers();

    void CreateIblTextureSet();

    void CreatePipelines();

    void CreateSkyboxCube();

    void CreateFullScreenQuad();

    void CreateScreenPrimitiveMeshes();

    void DrawToShadowMaps(vk::CommandBuffer cmd, uint32_t bufferingIndex);

    void DrawDeferred(vk::CommandBuffer cmd, uint32_t bufferingIndex);

    void DrawForward(vk::CommandBuffer cmd, uint32_t bufferingIndex);

    void PostProcess(vk::CommandBuffer cmd, uint32_t bufferingIndex);

    void DrawToScreen(const vk::RenderPassBeginInfo *primaryRenderPassBeginInfo, vk::CommandBuffer cmd, uint32_t bufferingIndex);

    VulkanBase                 m_device;
    TextureCache               m_textureCache;
    PbrMaterialCache           m_pbrMaterialCache;
    SingleTextureMaterialCache m_singleTextureMaterialCache;
    MeshCache                  m_meshCache;

    ShadowContext         m_shadowContext;
    DeferredContext       m_deferredContext;
    PostProcessingContext m_toneMappingContext;

    ShadowMatrixCalculator m_shadowMatrixCalculator;

    RendererUniformData    m_rendererUniformData{};
    LightingUniformData    m_lightingUniformData{};
    VulkanUniformBufferSet m_uniformBufferSet;

    vk::DescriptorSetLayout m_iblTextureSetLayout;
    vk::DescriptorSet       m_iblTextureSet;

    // shadow pass
    VulkanPipeline m_shadowPipeline;

    // deferred pass
    VulkanPipeline m_basePipeline;
    VulkanPipeline m_skyboxPipeline;

    // forward pass
    VulkanPipeline m_combinePipeline;
    VulkanPipeline m_baseForwardPipeline;

    // post-processing
    VulkanPipeline m_postProcessingPipeline;

    // presentation
    VulkanPipeline m_presentPipeline;
    VulkanPipeline m_screenRectPipeline;
    VulkanPipeline m_screenLinePipeline;

    VulkanMesh m_skyboxCube;
    VulkanMesh m_fullScreenQuad;
    VulkanMesh m_screenRectMesh;
    VulkanMesh m_screenLineMesh;

    std::vector<PointLightData> m_pointLights;

    struct DrawCall {
        const VulkanMesh  *Mesh;
        glm::mat4          ModelMatrix;
        const PbrMaterial *Material;

        DrawCall(const VulkanMesh *mesh, const glm::mat4 &modelMatrix, const PbrMaterial *material)
            : Mesh(mesh)
            , ModelMatrix(modelMatrix)
            , Material(material) {}
    };

    std::vector<DrawCall> m_deferredDrawCalls;
    std::vector<DrawCall> m_forwardDrawCalls;

    struct ScreenRectDrawCall {
        glm::vec2              PMin;
        glm::vec2              PMax;
        glm::vec2              UvMin;
        glm::vec2              UvMax;
        glm::vec4              Color;
        SingleTextureMaterial *Texture;

        ScreenRectDrawCall(
            const glm::vec2       &pMin,
            const glm::vec2       &pMax,
            const glm::vec2       &uvMin,
            const glm::vec2       &uvMax,
            const glm::vec4       &color,
            SingleTextureMaterial *texture
        )
            : PMin(pMin)
            , PMax(pMax)
            , UvMin(uvMin)
            , UvMax(uvMax)
            , Color(color)
            , Texture(texture) {}
    };

    static constexpr size_t SCREEN_RECT_DRAW_CALL_DATA_SIZE = sizeof(ScreenRectDrawCall) - sizeof(SingleTextureMaterial *);

    std::vector<ScreenRectDrawCall> m_screenRectDrawCalls;

    struct ScreenLineDrawCall {
        glm::vec2 P0;
        glm::vec2 P1;
        glm::vec4 Color;

        ScreenLineDrawCall(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec4 &color)
            : P0(p0)
            , P1(p1)
            , Color(color) {}
    };

    std::vector<ScreenLineDrawCall> m_screenLineDrawCalls;
};
