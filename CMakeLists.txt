# CMAKE VERSION
cmake_minimum_required(VERSION 3.15...3.17)

# Verify TARGET_API
if(NOT DEFINED TARGET_API)
	message(FATAL_ERROR "TARGET_API not specified")
elseif("${TARGET_API}" STREQUAL "Vulkan")
	set(ERM_VULKAN ON)
elseif("${TARGET_API}" STREQUAL "OpenGL")
	set(ERM_OPEN_GL ON)
elseif("${TARGET_API}" STREQUAL "DX12")
	set(ERM_DX12 ON)
else()
	message(FATAL_ERROR "Invalid target api: ${TARGET_API}")
endif()

# Verify PLATFORM
if(NOT DEFINED PLATFORM)
	message(FATAL_ERROR "PLATFORM not specified")
elseif("${PLATFORM}" STREQUAL "Windows")
	set(ERM_WINDOWS ON)
elseif("${PLATFORM}" STREQUAL "MacOS")
	set(ERM_MAC ON)
else()
	message(FATAL_ERROR "Invalid platform: ${PLATFORM}")
endif()

# Setup RTX
if(NOT DEFINED ERM_RAY_TRACING_ENABLED)
	set(ERM_RAY_TRACING_ENABLED OFF)
endif()

# Setup VERBOSE
if(NOT DEFINED ERM_VERBOSE)
	set(ERM_VERBOSE OFF)
endif()

# Setup Modules
set(ERM_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Modules/")

# Project declaration
project(
	"ERM_${TARGET_API}"
	VERSION 0.0.1
	DESCRIPTION "C++ Game Engine"
	LANGUAGES CXX
)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Install options
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/ERM/" CACHE STRING "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_INSTALL_PREFIX}/lib/Debug/" CACHE STRING "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_INSTALL_PREFIX}/lib/Release/" CACHE STRING "" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_INSTALL_PREFIX}/lib/Debug/" CACHE STRING "" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_INSTALL_PREFIX}/lib/Release/" CACHE STRING "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_INSTALL_PREFIX}/bin/Debug/" CACHE STRING "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_INSTALL_PREFIX}/bin/Release/" CACHE STRING "" FORCE)

file(MAKE_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}")
file(MAKE_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}")
file(MAKE_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}")
file(MAKE_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE}")
file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}")

# C++ Options
set(CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" CACHE STRING "" FORCE)

# Custom properties
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE STRING "" FORCE)
set(RES_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/res" CACHE STRING "" FORCE)
set(RES_DEST "${CMAKE_INSTALL_PREFIX}/res" CACHE STRING "" FORCE)

# IDEs friendly
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Setup CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" CACHE STRING "" FORCE)
include(macros)
include(ERMConfig)
include(AssimpConfig)

# Setup TARGET_API
setup_api()
message(STATUS "Using ${TARGET_API} APIs")

# Add dependencies based on TARGET_API
if(ERM_OPEN_GL)
	include(GLEWConfig)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/glew-cmake")
endif()

# Include configuration files for dependencies
include(TracyConfig)
include(GLMConfig)
include(GLFWConfig)
include(TinyXmlConfig)
include(FindFbx)

if(NOT ERM_ASSIMP_ENABLED)
	find_package(Fbx)
endif()

if(NOT FBX_FOUND)
	message(WARNING "Cannot find FBX libraries")
else()
	set(ERM_FBX_ENABLED ON)
endif()

# Adds common externals dependencies
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/tracy")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/glm")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/glfw")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/imgui")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/tinyxml")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/stb_image")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/SPIRV-Cross")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/fmod")

if(ERM_ASSIMP_ENABLED)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/assimp")
endif()

# Modules
add_subdirectory("${ERM_MODULES_DIR}/Engine")
add_subdirectory("${ERM_MODULES_DIR}/Main")

# Custom targets
set(PIPELINE_ARGS --res-dest ${RES_DEST} --res-src ${RES_SOURCE})

if(ERM_VULKAN)
	list(APPEND PIPELINE_ARGS --shaders-compiler ${SHADERS_COMPILER})
endif()

if(ERM_RAY_TRACING_ENABLED)
	list(APPEND PIPELINE_ARGS --rtx-enabled True)
endif()

set(PIPELINE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/resources_pipeline.py")

add_custom_target(
	RESOURCES_PIPELINE
	COMMAND ${PYTHON_EXECUTABLE} ${PIPELINE_SCRIPT} ${PIPELINE_ARGS}
	COMMENT "Resources Pipeline"
)
add_dependencies(ERM_Main RESOURCES_PIPELINE)

# Debug print variables
if(ERM_VERBOSE)
	get_cmake_property(VARIABLE_NAMES VARIABLES)
	list(SORT VARIABLE_NAMES)

	foreach(VARIABLE_NAME ${VARIABLE_NAMES})
    	message(STATUS "${VARIABLE_NAME}=${${VARIABLE_NAME}}")
	endforeach(VARIABLE_NAME ${VARIABLE_NAMES})

	execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")
endif()
