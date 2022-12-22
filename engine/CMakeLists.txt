find_package(Vulkan REQUIRED)

add_subdirectory(dependencies EXCLUDE_FROM_ALL)

add_library(engine
        include/core/Debug.h
        include/file/FileSystem.h
        include/file/JsonFile.h
        include/window/Window.h
        include/vulkan/DebugVk.h
        include/vulkan/ShaderCompiler.h
        include/vulkan/VulkanBuffer.h
        include/vulkan/VulkanImage.h
        include/vulkan/VulkanDevice.h
        include/vulkan/VulkanBase.h
        include/vulkan/VulkanMesh.h

        src/file/FileSystem.cpp
        src/window/Window.cpp
        src/vulkan/ShaderCompiler.cpp
        src/vulkan/vk_mem_alloc.cpp
        src/vulkan/VulkanBuffer.cpp
        src/vulkan/VulkanImage.cpp
        src/vulkan/VulkanDevice.cpp
        src/vulkan/VulkanBase.cpp
        src/vulkan/VulkanMesh.cpp)

target_include_directories(engine PUBLIC include)

target_compile_definitions(engine PUBLIC
        VULKAN_HPP_NO_EXCEPTIONS
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
