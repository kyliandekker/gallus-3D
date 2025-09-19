project(game)

# Gather and set IMGUI source files.
set(IMGUI
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_dx12.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_win32.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_helpers.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/implot.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/implot_items.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/ImGuizmo.cpp
)

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
add_executable(game WIN32 ${HEADERS} ${ICON} ${SOURCES} ${DX12} ${TINY_GLTF})

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
    Shcore.lib dxgi.lib d3d12.lib d3dcompiler.lib dxguid.lib Shlwapi.lib engine_noimgui.lib
)
target_link_libraries(game PRIVATE engine_noimgui)
target_link_directories(game PRIVATE
    "${CMAKE_SOURCE_DIR}/../$<CONFIG>"
)

if(MSVC)
    target_compile_options(game PRIVATE
        "$<$<CONFIG:${DEBUG}>:/Od>"   # Disable optimizations for Debug
        "$<$<CONFIG:${RELEASE}>:/O2>"  # Enable optimizations for Release
    )
endif()

# For GCC/Clang (if applicable), set optimization level to 0 for debugging
target_compile_options(game PRIVATE
    "$<$<CONFIG:${DEBUG}>:-O0>"  # Disable optimizations for Debug
    "$<$<CONFIG:${RELEASE}>:-O2>"  # Optimize for Release
)

if(MSVC)
    target_compile_options(game PRIVATE
        "$<$<CONFIG:${DEBUG}>:/MTd>"
        "$<$<CONFIG:${RELEASE}>:/MT>"
    )
endif()

if(MSVC)
    # Set Debug Information Format (/ZI) for Debug configuration
    set_property(TARGET game APPEND_STRING PROPERTY COMPILE_OPTIONS "/ZI")

    # Set the Linker Debug flag for /DEBUG (for Debug configurations)
    set_property(TARGET game APPEND_STRING PROPERTY LINK_OPTIONS "/DEBUG")
endif()