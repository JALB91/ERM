# CMAKE VERSION
cmake_minimum_required(VERSION 3.15...3.17)

# PROJECT DECLARATION
project("ERM_Engine")

# GATHER SOURCES
file(GLOB_RECURSE SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${TARGET_API}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${TARGET_API}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/${TARGET_API}/*"
)

if(NOT ERM_RAY_TRACING_ENABLED)
	list(FILTER SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
endif()
create_groups("${SOURCES}")

add_library("${PROJECT_NAME}" "${SOURCES}")

# Setup platform specific options
if(ERM_WINDOWS)
	include(MSVCConfig)
else()
	include(OSXConfig)
endif()
target_setup_project()

# Setup shader compiler args
if(SHADERS_COMPILER AND ERM_VULKAN)
	if(ERM_RAY_TRACING_ENABLED)
		set(SHADERS_COMPILER "${SHADERS_COMPILER} --target-spv=spv1.5 -I ${RES_DEST}/shaders/Vulkan/imports/")
	else()
		set(SHADERS_COMPILER "${SHADERS_COMPILER} --target-spv=spv1.3 -I ${RES_DEST}/shaders/Vulkan/imports/")
	endif()

    set(SHADERS_COMPILER "${SHADERS_COMPILER}" PARENT_SCOPE)
endif()

set(ERM_Engine_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/" "${CMAKE_CURRENT_SOURCE_DIR}/Sources/${TARGET_API}/")
set(ERM_Engine_INCLUDE_DIRS "${ERM_Engine_INCLUDE_DIRS}" PARENT_SCOPE)

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${TARGET_API_INCLUDE_DIR}"
		$<$<BOOL:${ERM_FBX_ENABLED}>:${FBX_INCLUDE_DIRS}>
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:${ASSIMP_INCLUDE_DIR}>
	PUBLIC
		"${ERM_Engine_INCLUDE_DIRS}"
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
		${TARGET_API_COMPILE_DEF}
	PRIVATE
		$<$<BOOL:${ERM_OPEN_GL}>:GLEW_STATIC>
		$<$<BOOL:${ERM_VULKAN}>:ERM_SHADER_COMPILER="${SHADERS_COMPILER}">
)

# TARGET COMMON FEATURES
target_compile_features(
	"${PROJECT_NAME}"
	PUBLIC 
		cxx_std_17
)

# TARGET COMMON PROPERTIES
set_target_properties(
	"${PROJECT_NAME}" 
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE 
		"${TARGET_API_LIB}"
		glfw
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:assimp>
		stb_image
		spirv-cross-cpp
		fmod_core
		fmod_studio
	PUBLIC 
        $<$<BOOL:${ERM_OPEN_GL}>:libglew_static>
		glm
		imgui
		tinyxml2
		Tracy::TracyClient
)

target_link_libraries(
	${PROJECT_NAME}
	LINK_PRIVATE
		debug "${FBX_LIBRARIES_DEBUG}"
		optimized "${FBX_LIBRARIES}"
)