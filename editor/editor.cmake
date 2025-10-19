project(editor)

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
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_toggle.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_toggle_palette.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_toggle_presets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_toggle_renderer.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/implot_items.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/ImGuizmo.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/ImSequencer.cpp
)

# Gather and set DX12 source files.
set(DX12
    ${CMAKE_SOURCE_DIR}/external/dx12/directx/d3dx12_property_format_table.cpp
)

# Gather and set tinygltf source files.
set(TINY_GLTF
    ${CMAKE_SOURCE_DIR}/external/tiny_gltf/tiny_gltf.cc
)

set(ICON ${CMAKE_SOURCE_DIR}/editor/src/Resource.rc)

# Gather all game files.
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/editor/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/editor/src/*.cpp)

# Define executable.
add_executable(editor WIN32 ${HEADERS} ${ICON} ${SOURCES} ${DX12} ${TINY_GLTF})

# Define preprocessor definitions for different configurations
target_compile_definitions(editor PRIVATE
    "$<$<CONFIG:${DEBUG}>:${PREDEFINITIONS_EDITOR_DEBUG}>"
    "$<$<CONFIG:${RELEASE}>:${PREDEFINITIONS_EDITOR_RELEASE}>"
)

# Include directories
target_include_directories(editor PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/src
    ${CMAKE_SOURCE_DIR}/editor/src
    ${CMAKE_SOURCE_DIR}/external
)

# Conditional editor sources
target_sources(editor PRIVATE
    $<$<CONFIG:${DEBUG}>:${IMGUI}>
    $<$<CONFIG:${RELEASE}>:${IMGUI}>
)

# Set C++ standard
set_target_properties(editor PROPERTIES
    CXX_STANDARD 20
)

target_link_libraries(editor PRIVATE
    Shcore.lib dxgi.lib d3d12.lib d3dcompiler.lib dxguid.lib Shlwapi.lib engine.lib
)
target_link_libraries(editor PRIVATE engine)
target_link_directories(editor PRIVATE
    "${CMAKE_SOURCE_DIR}/../$<CONFIG>"
)

if(MSVC)
    target_compile_options(editor PRIVATE
        "$<$<CONFIG:${DEBUG}>:/Od>"   # Disable optimizations for Debug
        "$<$<CONFIG:${RELEASE}>:/O2>"  # Enable optimizations for Release
    )
endif()

# For GCC/Clang (if applicable), set optimization level to 0 for debugging
target_compile_options(editor PRIVATE
    "$<$<CONFIG:${DEBUG}>:-O0>"  # Disable optimizations for Debug
    "$<$<CONFIG:${RELEASE}>:-O2>"  # Optimize for Release
)

if(MSVC)
    target_compile_options(editor PRIVATE
        "$<$<CONFIG:${DEBUG}>:/MTd>"
        "$<$<CONFIG:${RELEASE}>:/MT>"
    )
endif()

if(MSVC)
    # Set Debug Information Format (/ZI) for Debug configuration
    set_property(TARGET editor APPEND_STRING PROPERTY COMPILE_OPTIONS "/ZI")

    # Set the Linker Debug flag for /DEBUG (for Debug configurations)
    set_property(TARGET editor APPEND_STRING PROPERTY LINK_OPTIONS "/DEBUG")
endif()