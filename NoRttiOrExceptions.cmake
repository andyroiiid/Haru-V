# From https://github.com/KhronosGroup/glslang

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    string(FIND "${CMAKE_CXX_FLAGS}" "/GR" MSVC_HAS_GR)
    if (MSVC_HAS_GR)
        string(REGEX REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        add_compile_options(/GR-) # Disable RTTI
    endif ()

    string(REGEX REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # Try to remove default /EHsc cxx_flag
    add_compile_options(/D_HAS_EXCEPTIONS=0)
elseif ()
    add_compile_options(-fno-rtti)
    add_compile_options(-fno-exceptions)
endif ()
