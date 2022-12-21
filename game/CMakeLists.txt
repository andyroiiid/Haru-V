add_executable(game
        VertexFormats.cpp VertexFormats.h
        main.cpp)

target_include_directories(game PRIVATE .)

target_link_libraries(game PRIVATE engine)
