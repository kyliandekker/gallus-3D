project(imgui_system)

# Gather all imgui_system sources
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/imgui_system/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/imgui_system/src/*.cpp)

add_library(imgui_system STATIC ${HEADERS} ${SOURCES})

# Include directories
target_include_directories(imgui_system PUBLIC 
	${CMAKE_SOURCE_DIR}/imgui_system/src 
	${CMAKE_SOURCE_DIR}/engine/src 
	${CMAKE_SOURCE_DIR}/external
)

# Preprocessor definitions for config
target_compile_definitions(imgui_system PRIVATE
    "$<$<CONFIG:${DEBUG}>:${PREDEFINITIONS_EDITOR_DEBUG}>"
    "$<$<CONFIG:${RELEASE}>:${PREDEFINITIONS_EDITOR_RELEASE}>"
)

# C++ standard
set_target_properties(imgui_system PROPERTIES CXX_STANDARD 20)