cmake_minimum_required(VERSION 3.2)
project(fmod CXX)

set(FMOD_API "C:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows/api")

add_library(fmod SHARED IMPORTED GLOBAL)
set_target_properties(fmod PROPERTIES
        IMPORTED_LOCATION ${FMOD_API}/core/lib/x64/fmod.dll
        IMPORTED_IMPLIB ${FMOD_API}/core/lib/x64/fmod_vc.lib)
target_include_directories(fmod INTERFACE ${FMOD_API}/core/inc)

add_library(fmodstudio SHARED IMPORTED GLOBAL)
set_target_properties(fmodstudio PROPERTIES
        IMPORTED_LOCATION ${FMOD_API}/studio/lib/x64/fmodstudio.dll
        IMPORTED_IMPLIB ${FMOD_API}/studio/lib/x64/fmodstudio_vc.lib)
target_include_directories(fmodstudio INTERFACE ${FMOD_API}/studio/inc)

function(copy_fmod_dlls target)
    get_target_property(FMOD_DLL fmod IMPORTED_LOCATION)
    get_target_property(FMODSTUDIO_DLL fmodstudio IMPORTED_LOCATION)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${FMOD_DLL} $<TARGET_FILE_DIR:${target}>)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${FMODSTUDIO_DLL} $<TARGET_FILE_DIR:${target}>)
endfunction()
