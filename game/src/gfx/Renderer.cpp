//
// Created by andyroiiid on 12/21/2022.
//

#include "gfx/Renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <vulkan/ShaderCompiler.h>

#include "gfx/MeshUtilities.h"

Renderer::Renderer(GLFWwindow *window)
    : m_device(window)
    , m_textureCache(m_device)
    , m_pbrMaterialCache(m_device, m_textureCache)
    , m_meshCache(m_device) {
    m_shadowContext   = ShadowContext(m_device);
    m_deferredContext = DeferredContext(m_device);
    CreateUniformBuffers();
    CreateIblTextureSet();
    CreatePipelines();
    CreateSkyboxCube();
    CreateFullScreenQuad();
}

void Renderer::CreateUniformBuffers() {
    m_uniformBufferSet = VulkanUniformBufferSet(
        m_device,
        {
            {0, vk::ShaderStageFlagBits::eAllGraphics,                                   sizeof(RendererUniformData)},
            {1, vk::ShaderStageFlagBits::eGeometry | vk::ShaderStageFlagBits::eFragment, sizeof(LightingUniformData)}
    }
    );
}

void Renderer::CreateIblTextureSet() {
    vk::DescriptorSetLayoutBinding iblBindings[]{
        {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
        {1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
        {2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
        {3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_iblTextureSetLayout = m_device.CreateDescriptorSetLayout(iblBindings);

    m_iblTextureSet = m_device.AllocateDescriptorSet(m_iblTextureSetLayout);
    m_textureCache
        .LoadTexture(
            "textures/brdf_lut.png", //
            vk::Filter::eLinear,
            vk::SamplerAddressMode::eClampToEdge
        )
        ->BindToDescriptorSet(m_iblTextureSet, 0);
    m_textureCache
        .LoadTexture(
            "textures/ibl/sunset.png", //
            vk::Filter::eLinear,
            vk::SamplerAddressMode::eClampToEdge
        )
        ->BindToDescriptorSet(m_iblTextureSet, 1);
    m_textureCache
        .LoadTexture(
            "textures/ibl/sunset_specular.png", //
            vk::Filter::eLinear,
            vk::SamplerAddressMode::eClampToEdge
        )
        ->BindToDescriptorSet(m_iblTextureSet, 2);
    m_textureCache
        .LoadTexture(
            "textures/ibl/sunset_irradiance.png", //
            vk::Filter::eLinear,
            vk::SamplerAddressMode::eClampToEdge
        )
        ->BindToDescriptorSet(m_iblTextureSet, 3);
}

void Renderer::CreatePipelines() {
    const vk::PipelineColorBlendAttachmentState NO_BLEND(
        VK_FALSE,
        vk::BlendFactor::eZero,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eZero,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    );

    ShaderCompiler compiler("shader_includes/");

    m_shadowPipeline = VulkanPipeline(
        m_device,
        compiler,
        {
            m_uniformBufferSet.GetDescriptorSetLayout()
    },
        {
            {vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4)} //
        },
        VertexBase::GetPipelineVertexInputStateCreateInfo(),
        "pipelines/shadow.json",
        {},
        m_shadowContext.GetRenderPass(),
        0
    );

    m_basePipeline = VulkanPipeline(
        m_device,
        compiler,
        {
            m_uniformBufferSet.GetDescriptorSetLayout(),
            m_pbrMaterialCache.GetDescriptorSetLayout()
    },
        {
            {vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4)} //
        },
        VertexBase::GetPipelineVertexInputStateCreateInfo(),
        "pipelines/base.json",
        {NO_BLEND, NO_BLEND, NO_BLEND, NO_BLEND},
        m_deferredContext.GetDeferredRenderPass(),
        0
    );

    m_skyboxPipeline = VulkanPipeline(
        m_device,
        compiler,
        {m_uniformBufferSet.GetDescriptorSetLayout(), m_iblTextureSetLayout},
        {},
        VertexPositionOnly::GetPipelineVertexInputStateCreateInfo(),
        "pipelines/skybox.json",
        {NO_BLEND, NO_BLEND, NO_BLEND, NO_BLEND},
        m_deferredContext.GetDeferredRenderPass(),
        0
    );

    m_combinePipeline = VulkanPipeline(
        m_device,
        compiler,
        {m_uniformBufferSet.GetDescriptorSetLayout(),
         m_deferredContext.GetDeferredTextureSetLayout(),
         m_iblTextureSetLayout,
         m_shadowContext.GetTextureSetLayout()},
        {},
        VertexCanvas::GetPipelineVertexInputStateCreateInfo(),
        "pipelines/combine.json",
        {NO_BLEND},
        m_deferredContext.GetForwardRenderPass(),
        0
    );

    m_baseForwardPipeline = VulkanPipeline(
        m_device,
        compiler,
        {
            m_uniformBufferSet.GetDescriptorSetLayout(),
            m_pbrMaterialCache.GetDescriptorSetLayout(),
            m_iblTextureSetLayout,
            m_shadowContext.GetTextureSetLayout()
    },
        {
            {vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4)} //
        },
        VertexBase::GetPipelineVertexInputStateCreateInfo(),
        "pipelines/base_forward.json",
        {NO_BLEND},
        m_deferredContext.GetForwardRenderPass(),
        0
    );

    m_postProcessingPipeline = VulkanPipeline(
        m_device,
        compiler,
        {m_deferredContext.GetForwardTextureSetLayout()},
        {},
        VertexCanvas::GetPipelineVertexInputStateCreateInfo(),
        "pipelines/post_processing.json",
        {NO_BLEND},
        m_device.GetPrimaryRenderPass(),
        0
    );
}

void Renderer::CreateSkyboxCube() {
    const std::vector<VertexPositionOnly> vertices = CreateSkyboxVertices();

    m_skyboxCube = VulkanMesh(m_device, vertices.size(), sizeof(VertexPositionOnly), vertices.data());
}

void Renderer::CreateFullScreenQuad() {
    std::vector<VertexCanvas> vertices;
    AppendRectVertices(vertices, {-1.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f});
    m_fullScreenQuad = VulkanMesh(m_device, vertices.size(), sizeof(VertexCanvas), vertices.data());
}

Renderer::~Renderer() {
    m_device.WaitIdle();

    m_fullScreenQuad         = {};
    m_skyboxCube             = {};
    m_shadowPipeline         = {};
    m_basePipeline           = {};
    m_skyboxPipeline         = {};
    m_combinePipeline        = {};
    m_baseForwardPipeline    = {};
    m_postProcessingPipeline = {};
    m_device.FreeDescriptorSet(m_iblTextureSet);
    m_device.DestroyDescriptorSetLayout(m_iblTextureSetLayout);
    m_uniformBufferSet = {};
    m_deferredContext  = {};
    m_shadowContext    = {};
}

void Renderer::SetCameraData(const glm::vec3 &cameraPosition, const glm::mat4 &view, float fov, float near, float far) {
    const vk::Extent2D &extent      = m_device.GetSwapchainExtent();
    const float         aspectRatio = static_cast<float>(extent.width) / static_cast<float>(extent.height);

    m_rendererUniformData.Projection     = glm::perspective(fov, aspectRatio, near, far);
    m_rendererUniformData.View           = view;
    m_rendererUniformData.CameraPosition = cameraPosition;

    m_shadowMatrixCalculator.SetCameraInfo(view, fov, aspectRatio);
}

void Renderer::SetLightingData(const glm::vec3 &lightDirection, const glm::vec3 &lightColor) {
    m_lightingUniformData.LightDirection = lightDirection;
    m_lightingUniformData.LightColor     = lightColor;

    m_shadowMatrixCalculator.SetLightDirection(lightDirection);
}

void Renderer::SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max) {
    static constexpr float SHADOW_SAFE_DISTANCE = 4.0f;
    m_shadowMatrixCalculator.SetWorldBounds(min - SHADOW_SAFE_DISTANCE, max + SHADOW_SAFE_DISTANCE);
}

void Renderer::FinishDrawing() {
    const VulkanFrameInfo frameInfo = m_device.BeginFrame();

    m_deferredContext.CheckFramebuffersOutOfDate();

    // update shadow data
    constexpr float     shadowNear = 0.01f;
    constexpr float     shadowFar  = 64.0f;
    constexpr glm::vec3 csmSplits{8.0f, 16.0f, 32.0f};
    m_lightingUniformData.CascadeShadowMapSplits = csmSplits;
    m_lightingUniformData.ShadowMatrices[0]      = m_shadowMatrixCalculator.CalcShadowMatrix(shadowNear, csmSplits[0]);
    m_lightingUniformData.ShadowMatrices[1]      = m_shadowMatrixCalculator.CalcShadowMatrix(csmSplits[0], csmSplits[1]);
    m_lightingUniformData.ShadowMatrices[2]      = m_shadowMatrixCalculator.CalcShadowMatrix(csmSplits[1], csmSplits[2]);
    m_lightingUniformData.ShadowMatrices[3]      = m_shadowMatrixCalculator.CalcShadowMatrix(csmSplits[2], shadowFar);

    // update point lights
    const int numPointLights             = static_cast<int>(m_pointLights.size());
    m_lightingUniformData.NumPointLights = numPointLights;
    memcpy(m_lightingUniformData.PointLights, m_pointLights.data(), numPointLights * sizeof(PointLightData));
    m_pointLights.clear();

    m_uniformBufferSet.UpdateAllBuffers(frameInfo.BufferingIndex, {&m_rendererUniformData, &m_lightingUniformData});

    DrawToShadowMaps(frameInfo.CommandBuffer, frameInfo.BufferingIndex);
    DrawDeferred(frameInfo.CommandBuffer, frameInfo.BufferingIndex);
    DrawForward(frameInfo.CommandBuffer, frameInfo.BufferingIndex);
    DrawToScreen(frameInfo.PrimaryRenderPassBeginInfo, frameInfo.CommandBuffer, frameInfo.BufferingIndex);

    m_device.EndFrame();
}

void Renderer::DrawToShadowMaps(vk::CommandBuffer cmd, uint32_t bufferingIndex) {
    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_shadowContext.GetExtent(), false);

    cmd.beginRenderPass(m_shadowContext.GetRenderPassBeginInfo(bufferingIndex), vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_shadowPipeline.Get());

    cmd.setViewport(0, viewport);
    cmd.setScissor(0, scissor);

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_shadowPipeline.GetLayout(),
        0,
        m_uniformBufferSet.GetDescriptorSet(),
        m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
    );

    for (const DrawCall &drawCall: m_deferredDrawCalls) {
        cmd.pushConstants(
            m_shadowPipeline.GetLayout(), //
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(glm::mat4),
            glm::value_ptr(drawCall.ModelMatrix)
        );
        drawCall.Mesh->BindAndDraw(cmd);
    }

    for (const DrawCall &drawCall: m_forwardDrawCalls) {
        cmd.pushConstants(
            m_shadowPipeline.GetLayout(), //
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(glm::mat4),
            glm::value_ptr(drawCall.ModelMatrix)
        );
        drawCall.Mesh->BindAndDraw(cmd);
    }

    cmd.endRenderPass();
}

void Renderer::DrawDeferred(vk::CommandBuffer cmd, uint32_t bufferingIndex) {
    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_deferredContext.GetExtent());

    cmd.beginRenderPass(m_deferredContext.GetDeferredRenderPassBeginInfo(bufferingIndex), vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_basePipeline.Get());

    cmd.setViewport(0, viewport);
    cmd.setScissor(0, scissor);

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_basePipeline.GetLayout(),
        0,
        m_uniformBufferSet.GetDescriptorSet(),
        m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
    );

    for (const DrawCall &drawCall: m_deferredDrawCalls) {
        cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, //
            m_basePipeline.GetLayout(),
            1,
            drawCall.Material->DescriptorSet,
            {}
        );
        cmd.pushConstants(
            m_basePipeline.GetLayout(), //
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(glm::mat4),
            glm::value_ptr(drawCall.ModelMatrix)
        );
        drawCall.Mesh->BindAndDraw(cmd);
    }
    m_deferredDrawCalls.clear();

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_skyboxPipeline.Get());

    cmd.setViewport(0, viewport);
    cmd.setScissor(0, scissor);

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_skyboxPipeline.GetLayout(),
        0,
        {m_uniformBufferSet.GetDescriptorSet(), m_iblTextureSet},
        m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
    );
    m_skyboxCube.BindAndDraw(cmd);

    cmd.endRenderPass();
}

void Renderer::DrawForward(vk::CommandBuffer cmd, uint32_t bufferingIndex) {
    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_device.GetSwapchainExtent());

    cmd.beginRenderPass(m_deferredContext.GetForwardRenderPassBeginInfo(bufferingIndex), vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_combinePipeline.Get());

    cmd.setViewport(0, viewport);
    cmd.setScissor(0, scissor);

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_combinePipeline.GetLayout(),
        0,
        {m_uniformBufferSet.GetDescriptorSet(),
         m_deferredContext.GetDeferredTextureSet(bufferingIndex),
         m_iblTextureSet,
         m_shadowContext.GetTextureSet(bufferingIndex)},
        m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
    );

    m_fullScreenQuad.BindAndDraw(cmd);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_baseForwardPipeline.Get());

    cmd.setViewport(0, viewport);
    cmd.setScissor(0, scissor);

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_baseForwardPipeline.GetLayout(),
        0,
        m_uniformBufferSet.GetDescriptorSet(),
        m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
    );

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_baseForwardPipeline.GetLayout(),
        2,
        {m_iblTextureSet, m_shadowContext.GetTextureSet(bufferingIndex)},
        {}
    );

    for (const DrawCall &drawCall: m_forwardDrawCalls) {
        cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, //
            m_baseForwardPipeline.GetLayout(),
            1,
            drawCall.Material->DescriptorSet,
            {}
        );
        cmd.pushConstants(
            m_baseForwardPipeline.GetLayout(), //
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(glm::mat4),
            glm::value_ptr(drawCall.ModelMatrix)
        );
        drawCall.Mesh->BindAndDraw(cmd);
    }
    m_forwardDrawCalls.clear();

    cmd.endRenderPass();
}

void Renderer::DrawToScreen(const vk::RenderPassBeginInfo *primaryRenderPassBeginInfo, vk::CommandBuffer cmd, uint32_t bufferingIndex) {
    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_device.GetSwapchainExtent());

    cmd.beginRenderPass(primaryRenderPassBeginInfo, vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_postProcessingPipeline.Get());

    cmd.setViewport(0, viewport);
    cmd.setScissor(0, scissor);

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_postProcessingPipeline.GetLayout(),
        0,
        {m_deferredContext.GetForwardTextureSet(bufferingIndex)},
        {}
    );
    m_fullScreenQuad.BindAndDraw(cmd);

    cmd.endRenderPass();
}
