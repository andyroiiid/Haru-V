add_executable(game
        include/gfx/ShadowMaps.h
        include/gfx/ShadowContext.h
        include/gfx/DeferredFramebuffer.h
        include/gfx/ForwardFramebuffer.h
        include/gfx/DeferredContext.h
        include/gfx/PbrMaterialCache.h
        include/gfx/MeshUtilities.h
        include/gfx/Renderer.h

        src/gfx/ShadowContext.cpp
        src/gfx/DeferredFramebuffer.cpp
        src/gfx/ForwardFramebuffer.cpp
        src/gfx/ShadowMaps.cpp
        src/gfx/DeferredContext.cpp
        src/gfx/PbrMaterialCache.cpp
        src/gfx/MeshUtilities.cpp
        src/gfx/Renderer.cpp

        include/physics/PhysicsSimulationEventCallback.h

        src/physics/PhysicsSimulationEventCallback.cpp

        include/actors/Actor.h
        include/actors/Scene.h
        include/actors/AWorldSpawn.h
        include/actors/AFuncBrush.h
        include/actors/AFuncMove.h
        include/actors/AFuncButton.h
        include/actors/AFuncPhys.h
        include/actors/ATriggerTest.h
        include/actors/ALightPoint.h
        include/actors/APlayerNoClip.h
        include/actors/APlayer.h
        include/actors/APropTestModel.h
        include/actors/APropPowerSphere.h

        src/actors/Scene.cpp
        src/actors/AWorldSpawn.cpp
        src/actors/AFuncBrush.cpp
        src/actors/AFuncMove.cpp
        src/actors/AFuncButton.cpp
        src/actors/AFuncPhys.cpp
        src/actors/ATriggerTest.cpp
        src/actors/ALightPoint.cpp
        src/actors/APlayerNoClip.cpp
        src/actors/APlayer.cpp
        src/actors/APropTestModel.cpp
        src/actors/APropPowerSphere.cpp

        include/map/MapData.h
        include/map/Brushes.h
        include/map/MapParser.h
        include/map/EntityLoaders.h
        include/map/LoadEntities.h

        src/map/MapData.cpp
        src/map/Brushes.cpp
        src/map/MapParser.cpp
        src/map/EntityLoaders.cpp
        src/map/LoadEntities.cpp

        include/JsonFiles.h
        include/Globals.h
        include/Game.h

        src/JsonFiles.cpp
        src/Globals.cpp
        src/Game.cpp
        src/main.cpp)

target_include_directories(game PRIVATE include)

target_link_libraries(game PRIVATE engine)

add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:game>/game)

copy_fmod_dlls(game)
