#include <file/FileSystem.h>
#include <window/Window.h>

#include "Game.h"
#include "JsonFiles.h"

int main() {
    FileSystem::Init();
    FileSystem::Mount("game", "/");
    {
        const GameConfig gameConfig("game.json");

        Window window(gameConfig.Name, gameConfig.Width, gameConfig.Height, gameConfig.Maximized);
        Game   game;

        game.ScheduleMapLoad(gameConfig.StartMap);

        window.MainLoop(&game);
    }
    FileSystem::Shutdown();
    return 0;
}
