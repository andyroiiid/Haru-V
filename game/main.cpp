#include <glm/gtc/matrix_transform.hpp>

#include "file/FileSystem.h"
#include "window/Window.h"
#include "vulkan/ShaderCompiler.h"
#include "vulkan/VulkanBase.h"
#include "vulkan/VulkanMesh.h"

#include "JsonFiles.h"
#include "VertexFormats.h"

class Game : public App {
public:
    void Init(GLFWwindow *window) override {
        m_device = std::make_unique<VulkanBase>(window);

        m_pipelineLayout = m_device->CreatePipelineLayout(
                {},
                {
                        {vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4)}
                }
        );

        ShaderCompiler shaderCompiler;
        const std::vector<uint32_t> vertexSpirv = shaderCompiler.CompileFromFile(vk::ShaderStageFlagBits::eVertex, "shaders/test.vert");
        const std::vector<uint32_t> fragmentSpirv = shaderCompiler.CompileFromFile(vk::ShaderStageFlagBits::eFragment, "shaders/test.frag");

        m_vertexShaderModule = m_device->CreateShaderModule(vertexSpirv);
        m_fragmentShaderModule = m_device->CreateShaderModule(fragmentSpirv);

        m_pipeline = m_device->CreatePipeline(
                m_device->GetPrimaryRenderPass(),
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

        const std::vector<VertexBase> vertices{
                {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
                {{1.0f,  -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
                {{0.0f,  1.0f,  0.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 1.0f}},
        };
        m_mesh = VulkanMesh(m_device.get(), vertices.size(), sizeof(VertexBase), vertices.data());
    }

    void Shutdown() override {
        m_device->WaitIdle();

        m_mesh = {};
        m_device->DestroyPipeline(m_pipeline);
        m_device->DestroyShaderModule(m_vertexShaderModule);
        m_device->DestroyShaderModule(m_fragmentShaderModule);
        m_device->DestroyPipelineLayout(m_pipelineLayout);
        m_device.reset();
    }

    void Frame(float deltaTime) override {
        const auto [primaryRenderPassBeginInfo, bufferingIndex, cmd] = m_device->BeginFrame();

        cmd.beginRenderPass(primaryRenderPassBeginInfo, vk::SubpassContents::eInline);

        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
        const vk::Extent2D &size = m_device->GetSwapchainExtent();
        const auto [viewport, scissor] = CalcViewportAndScissorFromExtent(size);
        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);
        const glm::mat4 projection = glm::perspective(
                glm::radians(75.0f),
                static_cast<float>(size.width) / static_cast<float>(size.height),
                0.01f,
                100.0f
        );
        const glm::mat4 view = glm::lookAt(
                glm::vec3{3.0f, 4.0f, -5.0f},
                glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{0.0f, 1.0f, 0.0f}
        );
        const glm::mat4 model{1.0f};
        const glm::mat4 matrix = projection * view * model;
        cmd.pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &matrix);
        m_mesh.BindAndDraw(cmd);

        cmd.endRenderPass();

        m_device->EndFrame();
    }

private:
    std::unique_ptr<VulkanBase> m_device;
    vk::PipelineLayout m_pipelineLayout;
    vk::ShaderModule m_vertexShaderModule;
    vk::ShaderModule m_fragmentShaderModule;
    vk::Pipeline m_pipeline;
    VulkanMesh m_mesh;
};

int main() {
    FileSystem::Init();
    FileSystem::Mount("assets", "/");
    {
        const GameConfig gameConfig("game.json");
        Window window(gameConfig.Name, gameConfig.Width, gameConfig.Height);
        Game game;
        window.MainLoop(&game);
    }
    FileSystem::Shutdown();
    return 0;
}
