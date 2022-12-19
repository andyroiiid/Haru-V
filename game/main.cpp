#include "file/FileSystem.h"
#include "file/JsonFile.h"
#include "window/Window.h"
#include "vulkan/VulkanDevice.h"

class Game : public App {
public:
    void Init(GLFWwindow *window) override {
        m_device = std::make_unique<VulkanDevice>(window);
    }

    void Shutdown() override {
        m_device.reset();
    }

    void Frame(float deltaTime) override {
        auto [primaryRenderPassBeginInfo, bufferingIndex, cmd] = m_device->BeginFrame();

        cmd.beginRenderPass(primaryRenderPassBeginInfo, vk::SubpassContents::eInline);

        cmd.endRenderPass();

        m_device->EndFrame();
    }

private:
    std::unique_ptr<VulkanDevice> m_device;
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
