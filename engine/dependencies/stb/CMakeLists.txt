project(stb C)

add_library(stb src/stb_image.c)

target_include_directories(stb PUBLIC include)