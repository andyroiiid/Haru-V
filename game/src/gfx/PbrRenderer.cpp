//
// Created by andyroiiid on 12/21/2022.
//

#include "gfx/PbrRenderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <vulkan/ShaderCompiler.h>

#include "gfx/MeshUtilities.h"

PbrRenderer::PbrRenderer(GLFWwindow *window)
    : m_device(window)
    , m_textureCache(m_device)
    , m_pbrMaterialCache(m_device, m_textureCache)
    , m_singleTextureMaterialCache(m_device, m_textureCache)
    , m_meshCache(m_device) {
    m_shadowContext      = ShadowContext(m_device);
    m_deferredContext    = DeferredContext(m_device);
    m_toneMappingContext = PostProcessingContext(m_device);
    CreateUniformBuffers();
    CreateIblTextureSet();
    CreatePipelines();
    CreateSkyboxCube();
    CreateFullScreenQuad();
    CreateScreenPrimitiveMeshes();
}

void PbrRenderer::CreateUniformBuffers() {
    m_uniformBufferSet = VulkanUniformBufferSet(
        m_device,
        {
            {0, vk::ShaderStageFlagBits::eAllGraphics,                                   sizeof(RendererUniformData)},
            {1, vk::ShaderStageFlagBits::eGeometry | vk::ShaderStageFlagBits::eFragment, sizeof(LightingUniformData)}
    }
    );
}

void PbrRenderer::CreateIblTextureSet() {
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

void PbrRenderer::CreatePipelines() {
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

    const vk::PipelineColorBlendAttachmentState ALPHA_BLEND(
        VK_TRUE,
        vk::BlendFactor::eSrcAlpha,
        vk::BlendFactor::eOneMinusSrcAlpha,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eOne,
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
        {ALPHA_BLEND},
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
        m_toneMappingContext.GetRenderPass(),
        0
    );

    m_presentPipeline = VulkanPipeline(
        m_device,
        compiler,
        {m_uniformBufferSet.GetDescriptorSetLayout(), m_toneMappingContext.GetTextureSetLayout()},
        {},
        VertexCanvas::GetPipelineVertexInputStateCreateInfo(),
        "pipelines/present.json",
        {NO_BLEND},
        m_device.GetPrimaryRenderPass(),
        0
    );

    m_screenRectPipeline = VulkanPipeline(
        m_device,
        compiler,
        {
            m_uniformBufferSet.GetDescriptorSetLayout(),
            m_singleTextureMaterialCache.GetDescriptorSetLayout()
    },
        {{vk::ShaderStageFlagBits::eVertex, 0, SCREEN_RECT_DRAW_CALL_DATA_SIZE}},
        VertexCanvas::GetPipelineVertexInputStateCreateInfo(),
        "pipelines/screen_rect.json",
        {ALPHA_BLEND},
        m_device.GetPrimaryRenderPass(),
        0
    );

    m_screenLinePipeline = VulkanPipeline(
        m_device,
        compiler,
        {
            m_uniformBufferSet.GetDescriptorSetLayout()
    },
        {
            {vk::ShaderStageFlagBits::eVertex, 0, sizeof(ScreenLineDrawCall)} //
        },
        VertexCanvas::GetPipelineVertexInputStateCreateInfo(),
        "pipelines/screen_line.json",
        {ALPHA_BLEND},
        m_device.GetPrimaryRenderPass(),
        0
    );
}

void PbrRenderer::CreateSkyboxCube() {
    const std::vector<VertexPositionOnly> vertices = CreateSkyboxVertices();

    m_skyboxCube = VulkanMesh(m_device, vertices.size(), sizeof(VertexPositionOnly), vertices.data());
}

void PbrRenderer::CreateFullScreenQuad() {
    std::vector<VertexCanvas> vertices;
    AppendRectVertices(vertices, {-1.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f});
    m_fullScreenQuad = VulkanMesh(m_device, vertices.size(), sizeof(VertexCanvas), vertices.data());
}

void PbrRenderer::CreateScreenPrimitiveMeshes() {
    static const VertexCanvas rectVertices[4]{
        {{0.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.0f, 1.0f}, {0.0f, 1.0f}},
        {{1.0f, 1.0f}, {1.0f, 1.0f}}
    };
    m_screenRectMesh = VulkanMesh(m_device, 4, sizeof(VertexCanvas), rectVertices);

    static const VertexCanvas lineVertices[2]{
        {{0.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f}, {1.0f, 1.0f}}
    };
    m_screenLineMesh = VulkanMesh(m_device, 2, sizeof(VertexCanvas), lineVertices);
}

PbrRenderer::~PbrRenderer() {
    m_device.WaitIdle();

    m_screenRectMesh         = {};
    m_screenLineMesh         = {};
    m_fullScreenQuad         = {};
    m_skyboxCube             = {};
    m_shadowPipeline         = {};
    m_basePipeline           = {};
    m_skyboxPipeline         = {};
    m_combinePipeline        = {};
    m_baseForwardPipeline    = {};
    m_postProcessingPipeline = {};
    m_presentPipeline        = {};
    m_screenRectPipeline     = {};
    m_screenLinePipeline     = {};
    m_device.FreeDescriptorSet(m_iblTextureSet);
    m_device.DestroyDescriptorSetLayout(m_iblTextureSetLayout);
    m_uniformBufferSet = {};
    m_deferredContext  = {};
    m_shadowContext    = {};
}

void PbrRenderer::SetCameraData(const glm::vec3 &cameraPosition, const glm::mat4 &view, float fov, float near, float far) {
    const vk::Extent2D &extent      = m_device.GetSwapchainExtent();
    const float         aspectRatio = static_cast<float>(extent.width) / static_cast<float>(extent.height);

    m_rendererUniformData.Projection     = glm::perspective(fov, aspectRatio, near, far);
    m_rendererUniformData.View           = view;
    m_rendererUniformData.CameraPosition = cameraPosition;

    m_shadowMatrixCalculator.SetCameraInfo(view, fov, aspectRatio);
}

void PbrRenderer::SetLightingData(const glm::vec3 &lightDirection, const glm::vec3 &lightColor) {
    m_lightingUniformData.LightDirection = lightDirection;
    m_lightingUniformData.LightColor     = lightColor;

    m_shadowMatrixCalculator.SetLightDirection(lightDirection);
}

void PbrRenderer::SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max) {
    static constexpr float SHADOW_SAFE_DISTANCE = 4.0f;
    m_shadowMatrixCalculator.SetWorldBounds(min - SHADOW_SAFE_DISTANCE, max + SHADOW_SAFE_DISTANCE);
}

void PbrRenderer::FinishDrawing() {
    const VulkanFrameInfo frameInfo = m_device.BeginFrame();

    m_deferredContext.CheckFramebuffersOutOfDate();
    m_toneMappingContext.CheckFramebuffersOutOfDate();

    // update raw screen size
    {
        const vk::Extent2D &size           = m_device.GetSwapchainExtent();
        m_rendererUniformData.ScreenInfo.x = static_cast<float>(size.width);
        m_rendererUniformData.ScreenInfo.y = static_cast<float>(size.height);
        m_rendererUniformData.ScreenInfo.z = 1.0f / m_rendererUniformData.ScreenInfo.x;
        m_rendererUniformData.ScreenInfo.w = 1.0f / m_rendererUniformData.ScreenInfo.y;
    }

    // update scaled screen size (for world rendering)
    {
        const vk::Extent2D scaledSize            = m_device.GetScaledExtent();
        m_rendererUniformData.ScaledScreenInfo.x = static_cast<float>(scaledSize.width);
        m_rendererUniformData.ScaledScreenInfo.y = static_cast<float>(scaledSize.height);
        m_rendererUniformData.ScaledScreenInfo.z = 1.0f / m_rendererUniformData.ScaledScreenInfo.x;
        m_rendererUniformData.ScaledScreenInfo.w = 1.0f / m_rendererUniformData.ScaledScreenInfo.y;
    }

    // update shadow data
    {
        constexpr float     shadowNear = 0.01f;
        constexpr float     shadowFar  = 64.0f;
        constexpr glm::vec3 csmSplits{8.0f, 16.0f, 32.0f};
        m_lightingUniformData.CascadeShadowMapSplits = csmSplits;
        m_lightingUniformData.ShadowMatrices[0]      = m_shadowMatrixCalculator.CalcShadowMatrix(shadowNear, csmSplits[0]);
        m_lightingUniformData.ShadowMatrices[1]      = m_shadowMatrixCalculator.CalcShadowMatrix(csmSplits[0], csmSplits[1]);
        m_lightingUniformData.ShadowMatrices[2]      = m_shadowMatrixCalculator.CalcShadowMatrix(csmSplits[1], csmSplits[2]);
        m_lightingUniformData.ShadowMatrices[3]      = m_shadowMatrixCalculator.CalcShadowMatrix(csmSplits[2], shadowFar);
    }

    // update point lights
    {
        const int numPointLights             = static_cast<int>(m_pointLights.size());
        m_lightingUniformData.NumPointLights = numPointLights;
        memcpy(m_lightingUniformData.PointLights, m_pointLights.data(), numPointLights * sizeof(PointLightData));
        m_pointLights.clear();
    }

    m_uniformBufferSet.UpdateAllBuffers(frameInfo.BufferingIndex, {&m_rendererUniformData, &m_lightingUniformData});

    DrawToShadowMaps(frameInfo.CommandBuffer, frameInfo.BufferingIndex);
    DrawDeferred(frameInfo.CommandBuffer, frameInfo.BufferingIndex);
    DrawForward(frameInfo.CommandBuffer, frameInfo.BufferingIndex);
    PostProcess(frameInfo.CommandBuffer, frameInfo.BufferingIndex);
    DrawToScreen(frameInfo.PrimaryRenderPassBeginInfo, frameInfo.CommandBuffer, frameInfo.BufferingIndex);

    m_device.EndFrame();
}

void PbrRenderer::DrawToShadowMaps(vk::CommandBuffer cmd, uint32_t bufferingIndex) {
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
        if (!drawCall.Material->Shadow) {
            continue;
        }

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
        if (!drawCall.Material->Shadow) {
            continue;
        }

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

void PbrRenderer::DrawDeferred(vk::CommandBuffer cmd, uint32_t bufferingIndex) {
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

void PbrRenderer::DrawForward(vk::CommandBuffer cmd, uint32_t bufferingIndex) {
    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_deferredContext.GetExtent());

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

void PbrRenderer::PostProcess(vk::CommandBuffer cmd, uint32_t bufferingIndex) {
    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_toneMappingContext.GetExtent());

    cmd.beginRenderPass(m_toneMappingContext.GetRenderPassBeginInfo(bufferingIndex), vk::SubpassContents::eInline);

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

void PbrRenderer::DrawToScreen(const vk::RenderPassBeginInfo *primaryRenderPassBeginInfo, vk::CommandBuffer cmd, uint32_t bufferingIndex) {
    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_device.GetSwapchainExtent());

    cmd.beginRenderPass(primaryRenderPassBeginInfo, vk::SubpassContents::eInline);

    {
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_presentPipeline.Get());

        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);

        cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_presentPipeline.GetLayout(),
            0,
            {m_uniformBufferSet.GetDescriptorSet(), m_toneMappingContext.GetTextureSet(bufferingIndex)},
            m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
        );

        m_fullScreenQuad.BindAndDraw(cmd);
    }

    {
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_screenRectPipeline.Get());

        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);

        cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_screenRectPipeline.GetLayout(),
            0,
            {m_uniformBufferSet.GetDescriptorSet()},
            m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
        );

        for (const ScreenRectDrawCall &screenRectDrawCall: m_screenRectDrawCalls) {
            cmd.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics, //
                m_screenRectPipeline.GetLayout(),
                1,
                screenRectDrawCall.Texture->DescriptorSet,
                {}
            );
            cmd.pushConstants(
                m_screenRectPipeline.GetLayout(), //
                vk::ShaderStageFlagBits::eVertex,
                0,
                SCREEN_RECT_DRAW_CALL_DATA_SIZE,
                &screenRectDrawCall
            );
            m_screenRectMesh.BindAndDraw(cmd);
        }
        m_screenRectDrawCalls.clear();
    }

    {
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_screenLinePipeline.Get());

        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);

        cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_screenLinePipeline.GetLayout(),
            0,
            {m_uniformBufferSet.GetDescriptorSet()},
            m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
        );

        for (const ScreenLineDrawCall &screenLineDrawCall: m_screenLineDrawCalls) {
            cmd.pushConstants(
                m_screenLinePipeline.GetLayout(), //
                vk::ShaderStageFlagBits::eVertex,
                0,
                sizeof(ScreenLineDrawCall),
                &screenLineDrawCall
            );
            m_screenLineMesh.BindAndDraw(cmd);
        }
        m_screenLineDrawCalls.clear();
    }

    cmd.endRenderPass();
}
