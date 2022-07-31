# Cmake version
cmake_minimum_required(VERSION 3.15...3.17)

# Verify ERM_TARGET_API
if(NOT DEFINED ERM_TARGET_API)
	message(FATAL_ERROR "ERM_TARGET_API not specified")
elseif("${ERM_TARGET_API}" STREQUAL "Vulkan")
	set(ERM_VULKAN ON)
elseif("${ERM_TARGET_API}" STREQUAL "OpenGL")
	set(ERM_OPEN_GL ON)
elseif("${ERM_TARGET_API}" STREQUAL "DX12")
	set(ERM_DX12 ON)
else()
	message(FATAL_ERROR "Invalid target api: ${ERM_TARGET_API}")
endif()

# Verify ERM_PLATFORM
if(NOT DEFINED ERM_PLATFORM)
	message(FATAL_ERROR "ERM_PLATFORM not specified")
elseif("${ERM_PLATFORM}" STREQUAL "Windows")
	set(ERM_WINDOWS ON)
elseif("${ERM_PLATFORM}" STREQUAL "MacOS")
	set(ERM_MAC ON)
else()
	message(FATAL_ERROR "Invalid platform: ${ERM_PLATFORM}")
endif()

# PROJECT DECLARATION
project(
	"ERM_${ERM_PLATFORM}_${ERM_TARGET_API}"
	VERSION 0.0.1
	DESCRIPTION "C++ Game Engine"
	LANGUAGES CXX
)

# Setup RTX
if(NOT DEFINED ERM_RAY_TRACING_ENABLED)
	set(ERM_RAY_TRACING_ENABLED OFF)
endif()

# Setup verbosity
if(NOT DEFINED ERM_VERBOSE)
	set(ERM_VERBOSE OFF)
endif()

# Setup modules path
set(ERM_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Modules/")

# Setup configuration types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Install options
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/ERM/" CACHE STRING "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_INSTALL_PREFIX}/lib/Debug/" CACHE STRING "" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_INSTALL_PREFIX}/lib/Debug/" CACHE STRING "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_INSTALL_PREFIX}/bin/Debug/" CACHE STRING "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_INSTALL_PREFIX}/lib/Release/" CACHE STRING "" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_INSTALL_PREFIX}/lib/Release/" CACHE STRING "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_INSTALL_PREFIX}/bin/Release/" CACHE STRING "" FORCE)

# C++ Options
set(CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" CACHE STRING "" FORCE)

# Custom properties
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE STRING "" FORCE)
set(ERM_RES_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/res")
set(ERM_RES_DEST "${CMAKE_INSTALL_PREFIX}/res")

# IDEs friendly
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Setup CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(macros)
include(ERMConfig)
include(AssimpConfig)

# Setup ERM_TARGET_API
erm_setup_api()
erm_setup_shaders_compiler()
message(STATUS "Using ${ERM_TARGET_API} APIs")

# Add dependencies based on ERM_TARGET_API
if(ERM_OPEN_GL)
	include(GLEWConfig)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/glew-cmake")
endif()

# Add dependencies based on platform
if(ERM_WINDOWS)
	include(MSVCConfig)
elseif(ERM_MAC)
	include(OSXConfig)
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

if(NOT FBX_FOUND AND NOT ERM_ASSIMP_ENABLED)
	message(WARNING "Cannot find FBX libraries")
elseif(FBX_FOUND)
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

# Add modules
add_subdirectory("${ERM_MODULES_DIR}")

# Setup resources pipelien
erm_setup_resources_pipeline()
