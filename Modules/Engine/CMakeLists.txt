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

# Setup platform specific options
if(ERM_WINDOWS)
	include(MSVCConfig)
else()
	include(OSXConfig)
endif()
erm_target_setup_project()

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

target_compile_definitions(
	"${PROJECT_NAME}"
	PUBLIC
		$<$<BOOL:${ERM_VULKAN}>:GLM_FORCE_DEPTH_ZERO_TO_ONE>
		$<$<BOOL:${ERM_VULKAN}>:IMGUI_DISABLE_OBSOLETE_FUNCTIONS>
		$<$<BOOL:${ERM_FLIP_PROJECTION}>:ERM_FLIP_PROJECTION>
		$<$<BOOL:${ERM_FLIP_VIEWPORT}>:ERM_FLIP_VIEWPORT>
		$<$<BOOL:${ERM_FBX_ENABLED}>:ERM_FBX_ENABLED>
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:ERM_ASSIMP_ENABLED>
		$<$<BOOL:${ERM_RAY_TRACING_ENABLED}>:ERM_RAY_TRACING_ENABLED>
		$<$<BOOL:${ERM_TRACY_ENABLED}>:ERM_TRACY_ENABLED>
		${ERM_TARGET_API_COMPILE_DEF}
	PRIVATE
		$<$<BOOL:${ERM_OPEN_GL}>:GLEW_STATIC>
		$<$<BOOL:${ERM_VULKAN}>:ERM_SHADERS_COMPILER="${ERM_SHADERS_COMPILER}">
)

# Target common features
target_compile_features(
	"${PROJECT_NAME}"
	PUBLIC 
		cxx_std_17
)

# Target common properties
set_target_properties(
	"${PROJECT_NAME}" 
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE 
		"${ERM_TARGET_API_LIB}"
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:assimp>
		glfw
		stb_image
		spirv-cross-cpp
		fmod_core
		fmod_studio
	PUBLIC
		$<$<BOOL:${ERM_TRACY_ENABLED}>:Tracy::TracyClient>
        $<$<BOOL:${ERM_OPEN_GL}>:libglew_static>
		glm
		imgui
		tinyxml2
)

if(FBX_FOUND)
	target_link_libraries(
		${PROJECT_NAME}
		LINK_PRIVATE
			debug "${FBX_LIBRARIES_DEBUG}"
			optimized "${FBX_LIBRARIES}"
	)
endif()