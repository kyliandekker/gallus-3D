cmake_minimum_required(VERSION 3.20)

set(CMAKE_GENERATOR_PLATFORM x64)
project(Gallus)

# Configurations
set(RELEASE "Release")
set(DEBUG "Debug")
set(CMAKE_CONFIGURATION_TYPES "${RELEASE};${DEBUG}" CACHE STRING "" FORCE)
set(CMAKE_BUILD_TYPE Debug)

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG" CACHE STRING "Debug linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/OPT:REF /OPT:ICF" CACHE STRING "Release linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")

# Predefinitions
set(PREDEFINITIONS_SHARED "_LOAD_BY_PATH;RAPIDJSON_NOMEMBERITERATORCLASS;TINYGLTF_NO_INCLUDE_STB_IMAGE;TINYGLTF_NO_STB_IMAGE;TINYGLTF_NO_STB_IMAGE_WRITE;")
set(PREDEFINITIONS_GAME "IMGUI_DISABLE;")
set(PREDEFINITIONS_DEBUG_SHARED "_DEBUG;${PREDEFINITIONS_SHARED}")
set(PREDEFINITIONS_RELEASE_SHARED "NDEBUG;${PREDEFINITIONS_SHARED}")
set(PREDEFINITIONS_EDITOR_SHARED "_EDITOR;IMGUI_DEFINE_MATH_OPERATORS;${PREDEFINITIONS_SHARED}")
set(PREDEFINITIONS_EDITOR_DEBUG "${PREDEFINITIONS_EDITOR_SHARED};${PREDEFINITIONS_DEBUG_SHARED};_RENDER_TEX")
set(PREDEFINITIONS_EDITOR_RELEASE "${PREDEFINITIONS_EDITOR_SHARED};${PREDEFINITIONS_RELEASE_SHARED};_RENDER_TEX")
set(PREDEFINITIONS_GAME_DEBUG "${PREDEFINITIONS_DEBUG_SHARED};${PREDEFINITIONS_GAME}")
set(PREDEFINITIONS_GAME_RELEASE "${PREDEFINITIONS_RELEASE_SHARED};${PREDEFINITIONS_GAME}")

# Global settings
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER CMakePredefinedTargets)

# Third-party libraries

# TinyGLTF
add_library(tiny_gltf STATIC ${CMAKE_SOURCE_DIR}/external/tiny_gltf/tiny_gltf.cc)
target_include_directories(tiny_gltf PUBLIC ${CMAKE_SOURCE_DIR}/external/tiny_gltf)
set_target_properties(tiny_gltf PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON FOLDER CMakePredefinedTargets)
target_compile_definitions(tiny_gltf PRIVATE
    "$<$<CONFIG:Debug>:TINYGLTF_NO_INCLUDE_STB_IMAGE;TINYGLTF_NO_STB_IMAGE;TINYGLTF_NO_STB_IMAGE_WRITE;RAPIDJSON_NOMEMBERITERATORCLASS;_LOAD_BY_PATH;_DEBUG;>"
    "$<$<CONFIG:Release>:TINYGLTF_NO_INCLUDE_STB_IMAGE;TINYGLTF_NO_STB_IMAGE;TINYGLTF_NO_STB_IMAGE_WRITE;RAPIDJSON_NOMEMBERITERATORCLASS;_LOAD_BY_PATH;NDEBUG;>"
)

# DX12 helper
add_library(dx12_helper STATIC ${CMAKE_SOURCE_DIR}/external/dx12/directx/d3dx12_property_format_table.cpp)
target_include_directories(dx12_helper PUBLIC ${CMAKE_SOURCE_DIR}/external/dx12)
set_target_properties(dx12_helper PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON FOLDER CMakePredefinedTargets)

# IMGUI
set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_dx12.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_win32.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
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
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/external/imgui)
set_target_properties(imgui PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON FOLDER CMakePredefinedTargets)

# Additional IMGUI helpers
set(IMGUI
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_helpers.cpp
)

# Include modules
include(engine/engine.cmake)
include(editor/editor.cmake)
include(game/game.cmake)