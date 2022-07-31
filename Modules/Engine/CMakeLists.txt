# Cmake version
cmake_minimum_required(VERSION 3.15...3.17)

# Project declaration
project(
	"ERM_Engine"
	VERSION 0.0.1
	DESCRIPTION "ERM Engine module"
	LANGUAGES CXX
)

# Gather sources
file(GLOB_RECURSE SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_TARGET_API}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_TARGET_API}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/${ERM_TARGET_API}/*"
)

if(NOT ERM_RAY_TRACING_ENABLED)
	list(FILTER SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
endif()
erm_create_groups("${SOURCES}")

add_library("${PROJECT_NAME}" "${SOURCES}")

erm_target_setup_project()
erm_target_setup_common_defaults()

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${ERM_TARGET_API_INCLUDE_DIR}"
		$<$<BOOL:${ERM_FBX_ENABLED}>:${FBX_INCLUDE_DIRS}>
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:${ASSIMP_INCLUDE_DIR}>
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_TARGET_API}/"
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE 
		"${ERM_TARGET_API_LIB}"
		$<$<BOOL:${ERM_TRACY_ENABLED}>:Tracy::TracyClient>
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:assimp>
		$<$<BOOL:${ERM_OPEN_GL}>:libglew_static>
		glfw
		stb_image
		spirv-cross-cpp
		tinyxml2
		imgui
		ERM_Math
		ERM_Audio
	PUBLIC
		ERM_Input
		ERM_Utils
)

if(FBX_FOUND)
	target_link_libraries(
		${PROJECT_NAME}
		LINK_PRIVATE
			debug "${FBX_LIBRARIES_DEBUG}"
			optimized "${FBX_LIBRARIES}"
	)
endif()