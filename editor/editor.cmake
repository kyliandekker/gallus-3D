project(editor)

# Icon resource
set(ICON ${CMAKE_SOURCE_DIR}/editor/src/Resource.rc)

# Gather all editor files
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/editor/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/editor/src/*.cpp)

# Editor executable
add_executable(editor WIN32 ${HEADERS} ${ICON} ${SOURCES})

# Preprocessor definitions
target_compile_definitions(editor PRIVATE
    _EDITOR
    _LOAD_BY_PATH
    "$<$<CONFIG:${DEBUG}>:${PREDEFINITIONS_EDITOR_DEBUG}>"
    "$<$<CONFIG:${RELEASE}>:${PREDEFINITIONS_EDITOR_RELEASE}>"
)

# Include directories
target_include_directories(editor PUBLIC ${CMAKE_SOURCE_DIR}/engine/src ${CMAKE_SOURCE_DIR}/editor/src ${CMAKE_SOURCE_DIR}/external)

# Conditional editor sources
target_sources(editor PRIVATE
    $<$<CONFIG:${DEBUG}>:${IMGUI}>
    $<$<CONFIG:${RELEASE}>:${IMGUI}>
)

# C++ standard
set_target_properties(editor PROPERTIES CXX_STANDARD 20)

# Link libraries
target_link_libraries(editor PRIVATE tiny_gltf dx12_helper imgui Shcore.lib dxgi.lib d3d12.lib d3dcompiler.lib dxguid.lib Shlwapi.lib engine)

# Link directories
target_link_directories(editor PRIVATE "${CMAKE_SOURCE_DIR}/../$<CONFIG>")