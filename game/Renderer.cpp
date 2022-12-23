//
// Created by andyroiiid on 12/21/2022.
//

#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "vulkan/ShaderCompiler.h"
#include "file/ImageFile.h"

Renderer::Renderer(GLFWwindow *window)
        : m_device(window) {
    CreateDeferredRenderPass();
    CreateDeferredFramebuffers();
    CreateUniformBuffers();
    CreateTextureSet();
    CreatePipelines();
}

void Renderer::CreateDeferredRenderPass() {
    m_deferredPass = m_device.CreateRenderPass(
            {
                    vk::Format::eR32G32B32A32Sfloat,
                    vk::Format::eR32G32B32A32Sfloat,
                    vk::Format::eR8G8B8A8Unorm
            },
            vk::Format::eD32Sfloat
    );
}

void Renderer::CreateDeferredFramebuffers() {
    m_deferredExtent = m_device.GetSwapchainExtent();
    const size_t numBuffering = m_device.GetNumBuffering();
    for (int i = 0; i < numBuffering; i++) {
        DeferredObjects &deferredObjects = m_deferredObjects.emplace_back();

        // attachments
        deferredObjects.WorldPositionAttachment = m_device.CreateImage(
                vk::Format::eR32G32B32A32Sfloat,
                m_deferredExtent,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                0,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        );
        deferredObjects.WorldNormalAttachment = m_device.CreateImage(
                vk::Format::eR32G32B32A32Sfloat,
                m_deferredExtent,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                0,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        );
        deferredObjects.DiffuseAttachment = m_device.CreateImage(
                vk::Format::eR8G8B8A8Unorm,
                m_deferredExtent,
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                0,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        );
        deferredObjects.DepthAttachment = m_device.CreateImage(
                vk::Format::eD32Sfloat,
                m_deferredExtent,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                0,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        );

        // attachment views
        deferredObjects.WorldPositionAttachmentView = m_device.CreateImageView(
                deferredObjects.WorldPositionAttachment.Get(),
                vk::Format::eR32G32B32A32Sfloat,
                vk::ImageAspectFlagBits::eColor
        );
        deferredObjects.WorldNormalAttachmentView = m_device.CreateImageView(
                deferredObjects.WorldNormalAttachment.Get(),
                vk::Format::eR32G32B32A32Sfloat,
                vk::ImageAspectFlagBits::eColor
        );
        deferredObjects.DiffuseAttachmentView = m_device.CreateImageView(
                deferredObjects.DiffuseAttachment.Get(),
                vk::Format::eR8G8B8A8Unorm,
                vk::ImageAspectFlagBits::eColor
        );
        deferredObjects.DepthAttachmentView = m_device.CreateImageView(
                deferredObjects.DepthAttachment.Get(),
                vk::Format::eD32Sfloat,
                vk::ImageAspectFlagBits::eDepth
        );

        // framebuffer
        deferredObjects.Framebuffer = m_device.CreateFramebuffer(
                m_deferredPass,
                {
                        deferredObjects.WorldPositionAttachmentView,
                        deferredObjects.WorldNormalAttachmentView,
                        deferredObjects.DiffuseAttachmentView,
                        deferredObjects.DepthAttachmentView
                },
                m_deferredExtent
        );
    }
}

void Renderer::CleanupDeferredFramebuffers() {
    for (const DeferredObjects &deferredObjects: m_deferredObjects) {
        m_device.DestroyFramebuffer(deferredObjects.Framebuffer);
        m_device.DestroyImageView(deferredObjects.DepthAttachmentView);
        m_device.DestroyImageView(deferredObjects.DiffuseAttachmentView);
        m_device.DestroyImageView(deferredObjects.WorldNormalAttachmentView);
        m_device.DestroyImageView(deferredObjects.WorldPositionAttachmentView);
    }
    m_deferredObjects.clear();
}

void Renderer::CreateUniformBuffers() {
    m_uniformBufferSet = VulkanUniformBufferSet(
            m_device,
            {
                    {0, vk::ShaderStageFlagBits::eVertex,   sizeof(RendererUniformData)},
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
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
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
                    NO_BLEND
            },
            m_deferredPass,
            0
    );

    m_finalPipeline = VulkanPipeline(
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
            "shaders/test.json",
            {
                    NO_BLEND
            },
            m_device.GetPrimaryRenderPass(),
            0
    );
}

Renderer::~Renderer() {
    m_deferredPipeline = {};
    m_finalPipeline = {};
    m_device.FreeDescriptorSet(m_textureSet);
    m_texture = {};
    m_device.DestroyDescriptorSetLayout(m_textureSetLayout);
    m_uniformBufferSet = {};
    CleanupDeferredFramebuffers();
    m_device.DestroyRenderPass(m_deferredPass);
}

void Renderer::DrawToScreen() {
    const auto [primaryRenderPassBeginInfo, bufferingIndex, cmd] = m_device.BeginFrame();

    if (m_device.GetSwapchainExtent() != m_deferredExtent) {
        m_device.WaitIdle();
        CleanupDeferredFramebuffers();
        CreateDeferredFramebuffers();
    }

    m_uniformBufferSet.UpdateAllBuffers(bufferingIndex, {
            &m_rendererUniformData,
            &m_lightingUniformData
    });

    {
        static vk::ClearValue clearValues[]{
                {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}},
                {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}},
                {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}},
                {vk::ClearDepthStencilValue{1.0f, 0}}
        };
        const vk::RenderPassBeginInfo beginInfo(
                m_deferredPass,
                m_deferredObjects[bufferingIndex].Framebuffer,
                {{0, 0}, m_deferredExtent},
                clearValues
        );
        cmd.beginRenderPass(beginInfo, vk::SubpassContents::eInline);

        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_deferredPipeline.Get());

        const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(m_deferredExtent);
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

        cmd.endRenderPass();
    }

    {
        cmd.beginRenderPass(primaryRenderPassBeginInfo, vk::SubpassContents::eInline);

        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_finalPipeline.Get());

        const vk::Extent2D &extent = m_device.GetSwapchainExtent();
        const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(extent);
        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);

        cmd.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                m_finalPipeline.GetLayout(),
                0,
                {
                        m_uniformBufferSet.GetDescriptorSet(),
                        m_textureSet
                },
                m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
        );

        for (const DrawCall &drawCall: m_drawCalls) {
            cmd.pushConstants(
                    m_finalPipeline.GetLayout(),
                    vk::ShaderStageFlagBits::eVertex,
                    0,
                    sizeof(glm::mat4),
                    glm::value_ptr(drawCall.ModelMatrix)
            );
            drawCall.Mesh->BindAndDraw(cmd);
        }

        cmd.endRenderPass();
    }

    m_drawCalls.clear();

    m_device.EndFrame();
}
