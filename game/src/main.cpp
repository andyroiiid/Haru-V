#include <file/FileSystem.h>
#include <window/Window.h>

#include "JsonFiles.h"
#include "Game.h"

int main() {
    FileSystem::Init();
    FileSystem::Mount("assets", "/");
    {
        const GameConfig gameConfig("game.json");
        Window window(gameConfig.Name, gameConfig.Width, gameConfig.Height, gameConfig.Maximized);
        Game game;
        window.MainLoop(&game);
    }
    FileSystem::Shutdown();
    return 0;
}
