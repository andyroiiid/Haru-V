//
// Created by andyroiiid on 12/21/2022.
//

#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "vulkan/ShaderCompiler.h"
#include "file/ImageFile.h"

#include "VertexFormats.h"

Renderer::Renderer(GLFWwindow *window)
        : m_device(window) {
    m_deferredContext = DeferredContext(m_device);
    CreateUniformBuffers();
    CreateTextureSet();
    CreatePipelines();
    CreateFullScreenQuad();
}

void Renderer::CreateUniformBuffers() {
    m_uniformBufferSet = VulkanUniformBufferSet(
            m_device,
            {
                    {0, vk::ShaderStageFlagBits::eAllGraphics,   sizeof(RendererUniformData)},
                    {1, vk::ShaderStageFlagBits::eFragment, sizeof(LightingUniformData)}
            }
    );
}

void Renderer::CreateTextureSet() {
    vk::DescriptorSetLayoutBinding bindings[]{
            {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_textureSetLayout = m_device.CreateDescriptorSetLayout(bindings);

    const ImageFile imageFile("textures/dev_2.png");
    m_texture = VulkanTexture(
            m_device,
            imageFile.GetWidth(),
            imageFile.GetHeight(),
            imageFile.GetData()
    );

    m_textureSet = m_device.AllocateDescriptorSet(m_textureSetLayout);

    m_texture.BindToDescriptorSet(m_textureSet, 0);
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
                    m_deferredContext.GetTextureSetLayout()
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
    m_deferredPipeline = {};
    m_combinePipeline = {};
    m_device.FreeDescriptorSet(m_textureSet);
    m_texture = {};
    m_device.DestroyDescriptorSetLayout(m_textureSetLayout);
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

    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_deferredContext.GetExtent());
    cmd.setViewport(0, viewport);
    cmd.setScissor(0, scissor);

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
                    m_deferredContext.GetTextureSet(bufferingIndex)
            },
            m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
    );

    m_fullScreenQuad.BindAndDraw(cmd);

    cmd.endRenderPass();
}
