add_executable(game
        include/gfx/ShadowMaps.h
        include/gfx/ShadowContext.h
        include/gfx/DeferredFramebuffer.h
        include/gfx/ForwardFramebuffer.h
        include/gfx/DeferredContext.h
        include/gfx/PostProcessingFramebuffer.h
        include/gfx/PostProcessingContext.h
        include/gfx/PbrMaterialCache.h
        include/gfx/SingleTextureMaterialCache.h
        include/gfx/MeshUtilities.h
        include/gfx/PbrRenderer.h
        include/gfx/BitmapTextRenderer.h

        src/gfx/ShadowMaps.cpp
        src/gfx/ShadowContext.cpp
        src/gfx/DeferredFramebuffer.cpp
        src/gfx/ForwardFramebuffer.cpp
        src/gfx/DeferredContext.cpp
        src/gfx/PostProcessingFramebuffer.cpp
        src/gfx/PostProcessingContext.cpp
        src/gfx/PbrMaterialCache.cpp
        src/gfx/SingleTextureMaterialCache.cpp
        src/gfx/MeshUtilities.cpp
        src/gfx/PbrRenderer.cpp
        src/gfx/BitmapTextRenderer.cpp

        include/physics/PhysicsSimulationEventCallback.h

        src/physics/PhysicsSimulationEventCallback.cpp

        include/components/PlayerMovement.h
        include/components/PlayerUse.h

        src/components/PlayerMovement.cpp
        src/components/PlayerUse.cpp

        include/actors/Actor.h
        include/actors/Scene.h
        include/actors/AWorldSpawn.h
        include/actors/AFuncBrush.h
        include/actors/AFuncMove.h
        include/actors/AFuncButton.h
        include/actors/AFuncPhys.h
        include/actors/ATrigger.h
        include/actors/ATriggerOnce.h
        include/actors/ALightPoint.h
        include/actors/APlayerNoClip.h
        include/actors/APlayer.h
        include/actors/APropTestModel.h
        include/actors/APropPowerSphere.h

        src/actors/Scene.cpp
        src/actors/AWorldSpawn.cpp
        src/actors/AFuncBrush.cpp
        src/actors/AFuncButton.cpp
        src/actors/AFuncPhys.cpp
        src/actors/ATrigger.cpp
        src/actors/AFuncMove.cpp
        src/actors/ALightPoint.cpp
        src/actors/APlayerNoClip.cpp
        src/actors/APlayer.cpp
        src/actors/APropTestModel.cpp
        src/actors/APropPowerSphere.cpp

        include/script/GameLua.h

        src/script/GameLua.cpp

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

        include/ui/GameHUD.h
        src/ui/GameHUD.cpp

        include/JsonFiles.h
        include/Globals.h
        include/Game.h

        src/JsonFiles.cpp
        src/Globals.cpp
        src/Game.cpp
        src/main.cpp)

target_include_directories(game PRIVATE include)

target_link_libraries(game PRIVATE engine)

add_custom_target(copy_game_assets)
add_custom_command(TARGET copy_game_assets POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -R $<TARGET_FILE_DIR:game>/game)
add_custom_command(TARGET copy_game_assets POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:game>/game)

add_dependencies(game copy_game_assets)

copy_fmod_dlls(game)
