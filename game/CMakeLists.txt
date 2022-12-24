add_executable(game
        JsonFiles.cpp JsonFiles.h
        VertexFormats.cpp VertexFormats.h
        DeferredFramebuffer.cpp DeferredFramebuffer.h
        DeferredContext.cpp DeferredContext.h
        Renderer.cpp Renderer.h
        MeshUtilities.cpp MeshUtilities.h
        Game.cpp Game.h
        main.cpp)

target_include_directories(game PRIVATE .)

target_link_libraries(game PRIVATE engine)
