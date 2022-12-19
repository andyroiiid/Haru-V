find_package(Vulkan REQUIRED)

add_subdirectory(dependencies EXCLUDE_FROM_ALL)

add_library(engine
        include/core/Debug.h
        include/core/DebugVk.h
        include/file/FileSystem.h
        include/file/JsonFile.h
        include/window/Window.h
        include/vulkan/VulkanDevice.h

        src/file/FileSystem.cpp
        src/window/Window.cpp
        src/vulkan/VulkanDevice.cpp)

target_include_directories(engine PUBLIC include)

target_compile_definitions(engine PUBLIC
        GLFW_INCLUDE_VULKAN
        GLM_FORCE_LEFT_HANDED
        GLM_FORCE_DEPTH_ZERO_TO_ONE)

target_link_libraries(engine PUBLIC
        fmt
        spdlog
        physfs-static
        simdjson
        glfw
        Vulkan::Vulkan
        glslang
        SPIRV
        glslang-default-resource-limits
        VulkanMemoryAllocator
        glm
        stb)
