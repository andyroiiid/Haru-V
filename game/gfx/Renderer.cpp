//
// Created by andyroiiid on 12/21/2022.
//

#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "vulkan/ShaderCompiler.h"
#include "file/ImageFile.h"
#include "file/HdrImageFile.h"

#include "VertexFormats.h"

Renderer::Renderer(GLFWwindow *window)
        : m_device(window) {
    m_deferredContext = DeferredContext(m_device);
    CreateUniformBuffers();
    CreateIblTextureSet();
    CreateTextureSet();
    CreatePipelines();
    CreateSkyboxCube();
    CreateFullScreenQuad();
}

void Renderer::CreateUniformBuffers() {
    m_uniformBufferSet = VulkanUniformBufferSet(
            m_device,
            {
                    {0, vk::ShaderStageFlagBits::eAllGraphics, sizeof(RendererUniformData)},
                    {1, vk::ShaderStageFlagBits::eFragment,    sizeof(LightingUniformData)}
            }
    );
}

static VulkanTexture LoadTexture(VulkanBase &device, const std::string &filename) {
    const ImageFile image(filename);
    return {device, image.GetWidth(), image.GetHeight(), image.GetData()};
}

static VulkanTexture LoadHdrTexture(VulkanBase &device, const std::string &filename) {
    const HdrImageFile image(filename);
    return {device, image.GetWidth(), image.GetHeight(), image.GetData()};
}

void Renderer::CreateIblTextureSet() {
    vk::DescriptorSetLayoutBinding bindings[]{
            {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
            {1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
            {2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
            {3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_iblTextureSetLayout = m_device.CreateDescriptorSetLayout(bindings);

    m_brdfLutTexture = LoadTexture(m_device, "textures/brdf_lut.png");
    m_skyboxTexture = LoadHdrTexture(m_device, "textures/skybox.hdr");
    m_skyboxSpecularTexture = LoadTexture(m_device, "textures/skybox_specular.png");
    m_skyboxIrradianceTexture = LoadTexture(m_device, "textures/skybox_irradiance.png");

    m_iblTextureSet = m_device.AllocateDescriptorSet(m_iblTextureSetLayout);

    m_brdfLutTexture.BindToDescriptorSet(m_iblTextureSet, 0);
    m_skyboxTexture.BindToDescriptorSet(m_iblTextureSet, 1);
    m_skyboxSpecularTexture.BindToDescriptorSet(m_iblTextureSet, 2);
    m_skyboxIrradianceTexture.BindToDescriptorSet(m_iblTextureSet, 3);
}

void Renderer::CreateTextureSet() {
    vk::DescriptorSetLayoutBinding bindings[]{
            {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
            {1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
            {2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
            {3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_textureSetLayout = m_device.CreateDescriptorSetLayout(bindings);

    m_albedoTexture = LoadTexture(m_device, "textures/boom_box_albedo.jpg");
    m_normalTexture = LoadTexture(m_device, "textures/boom_box_normal.jpg");
    m_mraTexture = LoadTexture(m_device, "textures/boom_box_mra.jpg");
    m_emissiveTexture = LoadTexture(m_device, "textures/boom_box_emissive.jpg");

    m_textureSet = m_device.AllocateDescriptorSet(m_textureSetLayout);

    m_albedoTexture.BindToDescriptorSet(m_textureSet, 0);
    m_normalTexture.BindToDescriptorSet(m_textureSet, 1);
    m_mraTexture.BindToDescriptorSet(m_textureSet, 2);
    m_emissiveTexture.BindToDescriptorSet(m_textureSet, 3);
}

void Renderer::CreatePipelines() {
    const vk::PipelineColorBlendAttachmentState NO_BLEND(
            VK_FALSE,
            vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
            vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    );

    ShaderCompiler compiler;

    m_deferredPipeline = VulkanPipeline(
            m_device,
            compiler,
            {
                    m_uniformBufferSet.GetDescriptorSetLayout(),
                    m_textureSetLayout
            },
            {
                    {vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4)}
            },
            VertexBase::GetPipelineVertexInputStateCreateInfo(),
            "shaders/base.json",
            {
                    NO_BLEND,
                    NO_BLEND,
                    NO_BLEND,
                    NO_BLEND
            },
            m_deferredContext.GetRenderPass(),
            0
    );

    m_skyboxPipeline = VulkanPipeline(
            m_device,
            compiler,
            {
                    m_uniformBufferSet.GetDescriptorSetLayout(),
                    m_iblTextureSetLayout
            },
            {
            },
            VertexPositionOnly::GetPipelineVertexInputStateCreateInfo(),
            "shaders/skybox.json",
            {
                    NO_BLEND,
                    NO_BLEND,
                    NO_BLEND,
                    NO_BLEND
            },
            m_deferredContext.GetRenderPass(),
            0
    );

    m_combinePipeline = VulkanPipeline(
            m_device,
            compiler,
            {
                    m_uniformBufferSet.GetDescriptorSetLayout(),
                    m_deferredContext.GetTextureSetLayout(),
                    m_iblTextureSetLayout
            },
            {},
            VertexCanvas::GetPipelineVertexInputStateCreateInfo(),
            "shaders/combine.json",
            {
                    NO_BLEND
            },
            m_device.GetPrimaryRenderPass(),
            0
    );
}

void Renderer::CreateSkyboxCube() {
    const std::vector<VertexPositionOnly> vertices{
            VertexPositionOnly{{-1.0f, 1.0f, 1.0f}},
            VertexPositionOnly{{1.0f, 1.0f, 1.0f}},
            VertexPositionOnly{{-1.0f, -1.0f, 1.0f}},
            VertexPositionOnly{{1.0f, -1.0f, 1.0f}},
            VertexPositionOnly{{1.0f, -1.0f, -1.0f}},
            VertexPositionOnly{{1.0f, 1.0f, 1.0f}},
            VertexPositionOnly{{1.0f, 1.0f, -1.0f}},
            VertexPositionOnly{{-1.0f, 1.0f, 1.0f}},
            VertexPositionOnly{{-1.0f, 1.0f, -1.0f}},
            VertexPositionOnly{{-1.0f, -1.0f, 1.0f}},
            VertexPositionOnly{{-1.0f, -1.0f, -1.0f}},
            VertexPositionOnly{{1.0f, -1.0f, -1.0f}},
            VertexPositionOnly{{-1.0f, 1.0f, -1.0f}},
            VertexPositionOnly{{1.0f, 1.0f, -1.0}}
    };
    m_skyboxCube = VulkanMesh(m_device, vertices.size(), sizeof(VertexPositionOnly), vertices.data());
}

void Renderer::CreateFullScreenQuad() {
    const std::vector<VertexCanvas> vertices{
            {{-1.0f, -1.0f}, {0.0f, 0.0f}},
            {{1.0f,  -1.0f}, {1.0f, 0.0f}},
            {{-1.0f, 1.0f},  {0.0f, 1.0f}},
            {{1.0f,  1.0f},  {1.0f, 1.0f}}
    };
    m_fullScreenQuad = VulkanMesh(m_device, vertices.size(), sizeof(VertexCanvas), vertices.data());
}

Renderer::~Renderer() {
    m_device.WaitIdle();

    m_fullScreenQuad = {};
    m_skyboxCube = {};
    m_deferredPipeline = {};
    m_skyboxPipeline = {};
    m_combinePipeline = {};
    m_device.FreeDescriptorSet(m_textureSet);
    m_albedoTexture = {};
    m_normalTexture = {};
    m_mraTexture = {};
    m_emissiveTexture = {};
    m_device.DestroyDescriptorSetLayout(m_textureSetLayout);
    m_device.FreeDescriptorSet(m_iblTextureSet);
    m_skyboxTexture = {};
    m_skyboxIrradianceTexture = {};
    m_brdfLutTexture = {};
    m_device.DestroyDescriptorSetLayout(m_iblTextureSetLayout);
    m_uniformBufferSet = {};
    m_deferredContext = {};
}

void Renderer::FinishDrawing() {
    const VulkanFrameInfo frameInfo = m_device.BeginFrame();

    m_deferredContext.CheckFramebuffersOutOfDate();

    m_uniformBufferSet.UpdateAllBuffers(frameInfo.BufferingIndex, {
            &m_rendererUniformData,
            &m_lightingUniformData
    });

    DrawToDeferredTextures(frameInfo.CommandBuffer, frameInfo.BufferingIndex);
    DrawToScreen(frameInfo.PrimaryRenderPassBeginInfo, frameInfo.CommandBuffer, frameInfo.BufferingIndex);

    m_device.EndFrame();
}

void Renderer::DrawToDeferredTextures(vk::CommandBuffer cmd, uint32_t bufferingIndex) {
    cmd.beginRenderPass(m_deferredContext.GetRenderPassBeginInfo(bufferingIndex), vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_deferredPipeline.Get());

    {
        const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_deferredContext.GetExtent());
        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);
    }

    cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_deferredPipeline.GetLayout(),
            0,
            {
                    m_uniformBufferSet.GetDescriptorSet(),
                    m_textureSet
            },
            m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
    );

    for (const DrawCall &drawCall: m_drawCalls) {
        cmd.pushConstants(
                m_deferredPipeline.GetLayout(),
                vk::ShaderStageFlagBits::eVertex,
                0,
                sizeof(glm::mat4),
                glm::value_ptr(drawCall.ModelMatrix)
        );
        drawCall.Mesh->BindAndDraw(cmd);
    }
    m_drawCalls.clear();

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_skyboxPipeline.Get());

    {
        const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_deferredContext.GetExtent());
        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);
    }

    cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_skyboxPipeline.GetLayout(),
            0,
            {
                    m_uniformBufferSet.GetDescriptorSet(),
                    m_iblTextureSet
            },
            m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
    );
    m_skyboxCube.BindAndDraw(cmd);

    cmd.endRenderPass();
}

void Renderer::DrawToScreen(const vk::RenderPassBeginInfo *primaryRenderPassBeginInfo, vk::CommandBuffer cmd, uint32_t bufferingIndex) {
    cmd.beginRenderPass(primaryRenderPassBeginInfo, vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_combinePipeline.Get());

    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_device.GetSwapchainExtent());
    cmd.setViewport(0, viewport);
    cmd.setScissor(0, scissor);

    cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_combinePipeline.GetLayout(),
            0,
            {
                    m_uniformBufferSet.GetDescriptorSet(),
                    m_deferredContext.GetTextureSet(bufferingIndex),
                    m_iblTextureSet
            },
            m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
    );

    m_fullScreenQuad.BindAndDraw(cmd);

    cmd.endRenderPass();
}
