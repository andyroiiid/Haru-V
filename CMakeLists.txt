cmake_minimum_required(VERSION 3.24)
project(Haru-V)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 17)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

include(NoRttiOrExceptions.cmake)

add_subdirectory(engine)

add_subdirectory(game)
