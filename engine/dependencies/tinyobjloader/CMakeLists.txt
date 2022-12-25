project(tinyobjloader CXX)

add_library(tinyobjloader src/tiny_obj_loader.cc)

target_include_directories(tinyobjloader PUBLIC include)