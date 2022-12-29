add_executable(game
        include/gfx/DeferredFramebuffer.h
        include/gfx/DeferredContext.h
        include/gfx/PbrMaterialCache.h
        include/gfx/MeshUtilities.h
        include/gfx/Renderer.h

        src/gfx/DeferredFramebuffer.cpp
        src/gfx/DeferredContext.cpp
        src/gfx/PbrMaterialCache.cpp
        src/gfx/MeshUtilities.cpp
        src/gfx/Renderer.cpp

        include/actors/Actor.h
        include/actors/Scene.h
        include/actors/AWorldSpawn.h
        include/actors/ALightWorld.h
        include/actors/APlayerNoClip.h
        include/actors/APropTestModel.h

        src/actors/Scene.cpp
        src/actors/AWorldSpawn.cpp
        src/actors/ALightWorld.cpp
        src/actors/APlayerNoClip.cpp
        src/actors/APropTestModel.cpp

        include/map/MapData.h
        include/map/Brushes.h
        include/map/MapParser.h
        include/map/LoadEntities.h

        src/map/MapData.cpp
        src/map/Brushes.cpp
        src/map/MapParser.cpp
        src/map/LoadEntities.cpp

        include/JsonFiles.h
        include/Globals.h
        include/Game.h

        src/JsonFiles.cpp
        src/Globals.cpp
        src/Game.cpp
        src/main.cpp)

target_include_directories(game PRIVATE include)

target_link_libraries(game PRIVATE engine)
