project(game_shared)

file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/game_shared/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/game_shared/src/*.cpp)

set(DX12
    ${CMAKE_SOURCE_DIR}/external/dx12/directx/d3dx12_property_format_table.cpp
)

add_library(game_shared STATIC ${HEADERS} ${SOURCES} ${DX12} ${TINY_GLTF})
target_compile_definitions(game_shared PRIVATE IMGUI_DISABLE)

target_include_directories(game_shared PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/src
    ${CMAKE_SOURCE_DIR}/external
)

# Define preprocessor definitions for different configurations
target_compile_definitions(game_shared PRIVATE
    "$<$<CONFIG:${DEBUG}>:${PREDEFINITIONS_GAME_DEBUG}>"
    "$<$<CONFIG:${RELEASE}>:${PREDEFINITIONS_GAME_RELEASE}>"
)

# Set C++ standard
set_target_properties(game_shared PROPERTIES
    CXX_STANDARD 20
)

target_link_libraries(game_shared PRIVATE
    Shcore.lib dxgi.lib d3d12.lib d3dcompiler.lib dxguid.lib Shlwapi.lib engine_noimgui.lib
)
target_link_directories(game_shared PRIVATE
    "${CMAKE_SOURCE_DIR}/../$<CONFIG>"
)

if(MSVC)
    target_compile_options(game_shared PRIVATE
        "$<$<CONFIG:${DEBUG}>:/Od>"   # Disable optimizations for Debug
        "$<$<CONFIG:${RELEASE}>:/O2>"  # Enable optimizations for Release
    )
endif()

# For GCC/Clang (if applicable), set optimization level to 0 for debugging
target_compile_options(game_shared PRIVATE
    "$<$<CONFIG:${DEBUG}>:-O0>"  # Disable optimizations for Debug
    "$<$<CONFIG:${RELEASE}>:-O2>"  # Optimize for Release
)

if(MSVC)
    target_compile_options(game_shared PRIVATE
        "$<$<CONFIG:${DEBUG}>:/MTd>"
        "$<$<CONFIG:${RELEASE}>:/MT>"
    )
endif()

if(MSVC)
    # Set Debug Information Format (/ZI) for Debug configuration
    set_property(TARGET game_shared APPEND_STRING PROPERTY COMPILE_OPTIONS "/ZI")

    # Set the Linker Debug flag for /DEBUG (for Debug configurations)
    set_property(TARGET game_shared APPEND_STRING PROPERTY LINK_OPTIONS "/DEBUG")
endif()