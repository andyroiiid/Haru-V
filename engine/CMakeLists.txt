find_package(Vulkan REQUIRED)

add_subdirectory(dependencies EXCLUDE_FROM_ALL)

add_library(engine
        include/core/Debug.h

        include/file/FileSystem.h
        include/file/BinaryParser.h
        include/file/JsonFile.h
        include/file/ImageFile.h
        src/file/FileSystem.cpp
        src/file/BinaryParser.cpp
        src/file/JsonFile.cpp
        src/file/ImageFile.cpp

        include/window/Window.h
        include/window/Mouse.h
        src/window/Window.cpp
        src/window/Mouse.cpp

        include/vulkan/DebugVk.h
        include/vulkan/ShaderCompiler.h
        include/vulkan/VulkanBuffer.h
        include/vulkan/VulkanImage.h
        include/vulkan/VulkanDevice.h
        include/vulkan/VulkanBase.h
        include/vulkan/VulkanUniformBufferSet.h
        include/vulkan/VulkanPipelineConfig.h
        include/vulkan/VulkanPipeline.h
        include/vulkan/VulkanTexture.h
        include/vulkan/TextureCache.h
        include/vulkan/VertexFormats.h
        include/vulkan/VulkanMesh.h
        include/vulkan/MeshCache.h
        src/vulkan/ShaderCompiler.cpp
        src/vulkan/vk_mem_alloc.cpp
        src/vulkan/VulkanBuffer.cpp
        src/vulkan/VulkanImage.cpp
        src/vulkan/VulkanDevice.cpp
        src/vulkan/VulkanBase.cpp
        src/vulkan/VulkanUniformBufferSet.cpp
        src/vulkan/VulkanPipelineConfig.cpp
        src/vulkan/VulkanPipeline.cpp
        src/vulkan/VulkanTexture.cpp
        src/vulkan/TextureCache.cpp
        src/vulkan/VertexFormats.cpp
        src/vulkan/VulkanMesh.cpp
        src/vulkan/MeshCache.cpp

        include/math/Transform.h
        include/math/ShadowMatrixCalculator.h
        src/math/ShadowMatrixCalculator.cpp

        include/physics/PhysicsErrorCallback.h
        include/physics/PhysicsUtilities.h
        include/physics/PhysicsLayer.h
        include/physics/PhysicsSystem.h
        include/physics/PhysicsScene.h
        src/physics/PhysicsErrorCallback.cpp
        src/physics/PhysicsLayer.cpp
        src/physics/PhysicsSystem.cpp
        src/physics/PhysicsScene.cpp

        include/audio/DebugFMOD.h
        include/audio/AudioInstance.h
        include/audio/AudioSystem.h
        src/audio/AudioInstance.cpp
        src/audio/AudioSystem.cpp

        include/script/LuaSandbox.h
        src/script/LuaSandbox.cpp)

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
        stb
        tinyobjloader
        PhysX
        lua
        fmod
        fmodstudio
        TracyClient)
