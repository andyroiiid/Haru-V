add_executable(game
        gfx/VertexFormats.cpp gfx/VertexFormats.h
        gfx/DeferredFramebuffer.cpp gfx/DeferredFramebuffer.h
        gfx/DeferredContext.cpp gfx/DeferredContext.h
        gfx/Renderer.cpp gfx/Renderer.h
        gfx/MeshUtilities.cpp gfx/MeshUtilities.h

        JsonFiles.cpp JsonFiles.h
        Game.cpp Game.h
        main.cpp)

target_include_directories(game PRIVATE .)

target_link_libraries(game PRIVATE engine)
