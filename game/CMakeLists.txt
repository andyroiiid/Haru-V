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
        include/actors/APropTest.h

        src/actors/Scene.cpp
        src/actors/APropTest.cpp

        include/JsonFiles.h
        include/Game.h

        src/JsonFiles.cpp
        src/Game.cpp
        src/main.cpp)

target_include_directories(game PRIVATE include)

target_link_libraries(game PRIVATE engine)
