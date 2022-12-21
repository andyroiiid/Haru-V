#include "file/FileSystem.h"
#include "file/JsonFile.h"
#include "window/Window.h"
#include "vulkan/ShaderCompiler.h"
#include "vulkan/VulkanBase.h"

class Game : public App {
public:
    void Init(GLFWwindow *window) override {
        m_device = std::make_unique<VulkanBase>(window);

        m_pipelineLayout = m_device->CreatePipelineLayout(
                {},
                {}
        );

        std::vector<uint32_t> vertexSpirv;
        std::vector<uint32_t> fragmentSpirv;

        ShaderCompiler shaderCompiler;
        DebugCheckCritical(
                shaderCompiler.Compile(
                        vk::ShaderStageFlagBits::eVertex,
                        R"GLSL(
#version 450

void main() {
    const vec3 POSITIONS[3] = vec3[3](
        vec3(-1, -1, 0),
        vec3(1, -1, 0),
        vec3(0, 1, 0)
    );
    gl_Position = vec4(POSITIONS[gl_VertexIndex], 1);
}
)GLSL",
                        vertexSpirv
                ),
                "Failed to compile vertex shader."
        );
        DebugCheckCritical(
                shaderCompiler.Compile(
                        vk::ShaderStageFlagBits::eFragment,
                        R"GLSL(
#version 450

layout (location = 0) out vec4 fColor;

void main() {
    fColor = vec4(1, 0, 0, 1);
}
)GLSL",
                        fragmentSpirv
                ),
                "Failed to compile fragment shader."
        );

        m_vertexShaderModule = m_device->CreateShaderModule(vertexSpirv);
        m_fragmentShaderModule = m_device->CreateShaderModule(fragmentSpirv);

        vk::PipelineVertexInputStateCreateInfo inputState;
        m_pipeline = m_device->CreatePipeline(
                m_device->GetPrimaryRenderPass(),
                0,
                m_pipelineLayout,
                &inputState,
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

    void Shutdown() override {
        m_device->WaitIdle();

        m_device->DestroyPipeline(m_pipeline);
        m_device->DestroyShaderModule(m_vertexShaderModule);
        m_device->DestroyShaderModule(m_fragmentShaderModule);
        m_device->DestroyPipelineLayout(m_pipelineLayout);
        m_device.reset();
    }

    void Frame(float deltaTime) override {
        auto [primaryRenderPassBeginInfo, bufferingIndex, cmd] = m_device->BeginFrame();

        cmd.beginRenderPass(primaryRenderPassBeginInfo, vk::SubpassContents::eInline);

        const vk::Extent2D &size = m_device->GetSwapchainExtent();
        // flipped upside down so that it's consistent with OpenGL
        vk::Viewport viewport(
                0.0f, static_cast<float>(size.height),
                static_cast<float>(size.width), -static_cast<float>(size.height),
                0.0f, 1.0f
        );
        vk::Rect2D scissor(
                {0, 0},
                size
        );

        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
        cmd.setViewport(0, viewport);
        cmd.setScissor(0, scissor);
        cmd.draw(3, 1, 0, 0);

        cmd.endRenderPass();

        m_device->EndFrame();
    }

private:
    std::unique_ptr<VulkanBase> m_device;
    vk::PipelineLayout m_pipelineLayout;
    vk::ShaderModule m_vertexShaderModule;
    vk::ShaderModule m_fragmentShaderModule;
    vk::Pipeline m_pipeline;
};

int main() {
    FileSystem::Init();
    FileSystem::Mount("assets", "/");
    {
        JsonFile gameJson("game.json");
        auto name = gameJson.GetField<std::string>("name");
        auto width = gameJson.GetField<int>("width");
        auto height = gameJson.GetField<int>("height");
        DebugInfo("name = {}, width = {}, height = {}", name, width, height);
        Window window(name, width, height);
        Game game;
        window.MainLoop(&game);
    }
    FileSystem::Shutdown();
    return 0;
}
