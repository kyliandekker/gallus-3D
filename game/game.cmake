project(game)

# Gather and set DX12 source files.
set(DX12
    ${CMAKE_SOURCE_DIR}/external/dx12/directx/d3dx12_property_format_table.cpp
)

# Gather and set tinygltf source files.
set(TINY_GLTF
    ${CMAKE_SOURCE_DIR}/external/tiny_gltf/tiny_gltf.cc
)

set(ICON ${CMAKE_SOURCE_DIR}/game/src/Resource.rc)

# Gather all game files.
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/game/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/game/src/*.cpp)

# Define executable.
add_executable(game WIN32 ${HEADERS} ${ICON} ${SOURCES})

# Define preprocessor definitions for different configurations
target_compile_definitions(game PRIVATE
    "$<$<CONFIG:${DEBUG}>:${PREDEFINITIONS_GAME_DEBUG}>"
    "$<$<CONFIG:${RELEASE}>:${PREDEFINITIONS_GAME_RELEASE}>"
)

# Include directories
target_include_directories(game PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/src
    ${CMAKE_SOURCE_DIR}/game/src
    ${CMAKE_SOURCE_DIR}/external
)

# Set C++ standard
set_target_properties(game PROPERTIES
    CXX_STANDARD 20
)

target_link_libraries(game PRIVATE
    tiny_gltf dx12_helper Shcore.lib dxgi.lib d3d12.lib d3dcompiler.lib dxguid.lib Shlwapi.lib engine_noimgui.lib
)
target_link_libraries(game PRIVATE engine_noimgui)
target_link_directories(game PRIVATE
    "${CMAKE_SOURCE_DIR}/../$<CONFIG>"
)