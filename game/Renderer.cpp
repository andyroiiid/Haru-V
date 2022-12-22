//
// Created by andyroiiid on 12/21/2022.
//

#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "vulkan/ShaderCompiler.h"

Renderer::Renderer(GLFWwindow *window)
        : m_device(window) {
    CreateDescriptorSetLayout();
    CreateBufferingObjects();
    CreatePipeline();
}

void Renderer::CreateDescriptorSetLayout() {
    m_rendererDescriptorSetLayout = m_device.CreateDescriptorSetLayout(
            {
                    {0, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eVertex}
            }
    );
}

void Renderer::CreateBufferingObjects() {
    const size_t numBuffering = m_device.GetNumBuffering();

    m_rendererUniformBuffer = m_device.CreateBuffer(
            numBuffering * sizeof(RendererUniformData),
            vk::BufferUsageFlagBits::eUniformBuffer,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_HOST
    );

    m_rendererDescriptorSet = m_device.AllocateDescriptorSet(m_rendererDescriptorSetLayout);

    const vk::DescriptorBufferInfo bufferInfo(
            m_rendererUniformBuffer.Get(),
            0,
            sizeof(RendererUniformData)
    );
    const vk::WriteDescriptorSet writeDescriptorSet(
            m_rendererDescriptorSet,
            0,
            0,
            vk::DescriptorType::eUniformBufferDynamic,
            {},
            bufferInfo
    );
    m_device.WriteDescriptorSet(writeDescriptorSet);
}

void Renderer::CreatePipeline() {
    m_pipelineLayout = m_device.CreatePipelineLayout(
            {
                    m_rendererDescriptorSetLayout
            },
            {
                    {vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4)}
            }
    );

    ShaderCompiler shaderCompiler;
    const std::vector<uint32_t> vertexSpirv = shaderCompiler.CompileFromFile(vk::ShaderStageFlagBits::eVertex, "shaders/test.vert");
    const std::vector<uint32_t> fragmentSpirv = shaderCompiler.CompileFromFile(vk::ShaderStageFlagBits::eFragment, "shaders/test.frag");
    m_vertexShaderModule = m_device.CreateShaderModule(vertexSpirv);
    m_fragmentShaderModule = m_device.CreateShaderModule(fragmentSpirv);

    m_pipeline = m_device.CreatePipeline(
            m_device.GetPrimaryRenderPass(),
            0,
            m_pipelineLayout,
            VertexBase::GetPipelineVertexInputStateCreateInfo(),
            {
                    {{}, vk::ShaderStageFlagBits::eVertex,   m_vertexShaderModule,   "main"},
                    {{}, vk::ShaderStageFlagBits::eFragment, m_fragmentShaderModule, "main"}
            },
            {},
            {
                    {
                            VK_FALSE,
                            vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
                            vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
                            vk::ColorComponentFlagBits::eR |
                            vk::ColorComponentFlagBits::eG |
                            vk::ColorComponentFlagBits::eB |
                            vk::ColorComponentFlagBits::eA
                    }
            }
    );
}

Renderer::~Renderer() {
    m_device.DestroyPipeline(m_pipeline);
    m_device.DestroyShaderModule(m_vertexShaderModule);
    m_device.DestroyShaderModule(m_fragmentShaderModule);
    m_device.DestroyPipelineLayout(m_pipelineLayout);

    m_device.FreeDescriptorSet(m_rendererDescriptorSet);
    m_rendererUniformBuffer = {};

    m_device.DestroyDescriptorSetLayout(m_rendererDescriptorSetLayout);
}

void Renderer::DrawToScreen() {
    const auto [primaryRenderPassBeginInfo, bufferingIndex, cmd] = m_device.BeginFrame();

    const uint32_t rendererUniformBufferOffset = bufferingIndex * sizeof(RendererUniformData);
    m_rendererUniformBuffer.UploadRange(rendererUniformBufferOffset, sizeof(RendererUniformData), &m_rendererUniformData);

    cmd.beginRenderPass(primaryRenderPassBeginInfo, vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);

    const vk::Extent2D &extent = m_device.GetSwapchainExtent();
    const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(extent);
    cmd.setViewport(0, viewport);
    cmd.setScissor(0, scissor);

    cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_pipelineLayout,
            0,
            m_rendererDescriptorSet,
            rendererUniformBufferOffset
    );

    for (const DrawCall &drawCall: m_drawCalls) {
        cmd.pushConstants(
                m_pipelineLayout,
                vk::ShaderStageFlagBits::eVertex,
                0,
                sizeof(glm::mat4),
                glm::value_ptr(drawCall.ModelMatrix)
        );
        drawCall.Mesh->BindAndDraw(cmd);
    }
    m_drawCalls.clear();

    cmd.endRenderPass();

    m_device.EndFrame();
}
