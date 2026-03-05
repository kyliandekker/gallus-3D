project(engine)

# Gather all engine sources
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/engine/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/engine/src/*.cpp)

# Engine libraries
add_library(engine STATIC ${HEADERS} ${SOURCES})

# Include directories
target_include_directories(engine PUBLIC 
	${CMAKE_SOURCE_DIR}/engine/src 
	${CMAKE_SOURCE_DIR}/external
)

# Preprocessor definitions for config
target_compile_definitions(engine PRIVATE
    "$<$<CONFIG:${DEBUG}>:${PREDEFINITIONS_EDITOR_DEBUG}>"
    "$<$<CONFIG:${RELEASE}>:${PREDEFINITIONS_EDITOR_RELEASE}>"
)

# C++ standard
set_target_properties(engine PROPERTIES CXX_STANDARD 20)