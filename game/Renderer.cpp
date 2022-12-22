//
// Created by andyroiiid on 12/21/2022.
//

#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "vulkan/ShaderCompiler.h"

Renderer::Renderer(GLFWwindow *window)
        : m_device(window) {
    CreateUniformBuffers();
    CreatePipeline();
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

void Renderer::CreatePipeline() {
    m_pipelineLayout = m_device.CreatePipelineLayout(
            {
                    m_uniformBufferSet.GetDescriptorSetLayout()
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

    m_uniformBufferSet = {};
}

void Renderer::DrawToScreen() {
    const auto [primaryRenderPassBeginInfo, bufferingIndex, cmd] = m_device.BeginFrame();

    m_uniformBufferSet.UpdateAllBuffers(bufferingIndex, {
            &m_rendererUniformData,
            &m_lightingUniformData
    });

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
            m_uniformBufferSet.GetDescriptorSet(),
            m_uniformBufferSet.GetDynamicOffsets(bufferingIndex)
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
