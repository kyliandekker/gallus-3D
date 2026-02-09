project(engine_noimgui)

# Gather all engine sources
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/engine/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/engine/src/*.cpp)

# Engine libraries
add_library(engine STATIC ${HEADERS} ${SOURCES})
target_compile_definitions(engine PRIVATE ENGINE_WITH_IMGUI _EDITOR _LOAD_BY_PATH)
add_library(engine_noimgui STATIC ${HEADERS} ${SOURCES})
target_compile_definitions(engine_noimgui PRIVATE IMGUI_DISABLE _LOAD_BY_PATH)
set_target_properties(engine_noimgui PROPERTIES FOLDER "Hidden")

# Include directories
target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/engine/src ${CMAKE_SOURCE_DIR}/external)
target_include_directories(engine_noimgui PUBLIC ${CMAKE_SOURCE_DIR}/engine/src ${CMAKE_SOURCE_DIR}/external)

# Preprocessor definitions for config
target_compile_definitions(engine PRIVATE
    "$<$<CONFIG:${DEBUG}>:${PREDEFINITIONS_EDITOR_DEBUG}>"
    "$<$<CONFIG:${RELEASE}>:${PREDEFINITIONS_EDITOR_RELEASE}>"
)
target_compile_definitions(engine_noimgui PRIVATE
    "$<$<CONFIG:${DEBUG}>:${PREDEFINITIONS_GAME_DEBUG}>"
    "$<$<CONFIG:${RELEASE}>:${PREDEFINITIONS_GAME_RELEASE}>"
)

# C++ standard
set_target_properties(engine PROPERTIES CXX_STANDARD 20)
set_target_properties(engine_noimgui PROPERTIES CXX_STANDARD 20)

# Link libraries
target_link_libraries(engine PRIVATE tiny_gltf dx12_helper imgui Shcore.lib dxgi.lib d3d12.lib d3dcompiler.lib dxguid.lib Shlwapi.lib)
target_link_libraries(engine_noimgui PRIVATE tiny_gltf dx12_helper Shcore.lib dxgi.lib d3d12.lib d3dcompiler.lib dxguid.lib Shlwapi.lib)
