# CMAKE VERSION
cmake_minimum_required(VERSION 3.1...3.17)

# PROJECT DECLARATION
project(
	"ERM"
	VERSION 0.0.1
	DESCRIPTION "C++ Game Engine"
	LANGUAGES CXX
)

# BUILD TYPE
if(NOT "${CMAKE_BUILD_TYPE}")
	set(CMAKE_BUILD_TYPE "Debug")
endif()

# C++ OPTIONS
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# SETUP CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(macros)
include(ERMConfig)

# SETUP TARGET API
setup_api()
message(STATUS "Using ${TARGET_API} APIs")

# ADD DEPENDENCIES BASED ON TARGET API
if ("${TARGET_API}" STREQUAL "OpenGl")
	include(GLEWConfig)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/glew-cmake")
	set(PUBLIC_DEPENDENCIES "libglew_static")
endif()

# INCLUDE CONFIGURATION FILES FOR DEPENDENCIES
include(GLFWConfig)
include(GLMConfig)

# ADDS COMMON EXTERNALS DEPENDENCIES
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/glm")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/glfw")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/imgui")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/stb_image")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/externals/tinyxml")

# GATHER SOURCES
file(GLOB_RECURSE SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
create_groups("${SOURCES}")

# CREATE EXECUTABLE IF STANDALONE, LIBRARY IF NOT
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
	add_executable("${PROJECT_NAME}" "${SOURCES}")
else()
	add_library("${PROJECT_NAME}" "${SOURCES}")
endif()

# LINK LIBRARIES
target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE 
		"${TARGET_API_LIB}"
		"${PRIVATE_DEPENDENCIES}"
		glfw
		stb_image
	PUBLIC 
		"${PUBLIC_DEPENDENCIES}"
		glm
		imgui
		tinyxml2
)

# TARGET INCLUDE DIRECTORIES
target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${PRIVATE_INCLUDES}"
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
	PUBLIC 
		"${PUBLIC_INCLUDES}"
)

# TARGET COMMON CONFIGURATIONS
target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE 
		IMGUI_IMPL_OPENGL_LOADER_GLEW
		GLEW_STATIC
)

# TARGET COMMON FEATURES
target_compile_features(
	"${PROJECT_NAME}"
	PUBLIC 
		cxx_std_17
)

# TARGET COMMON PROPERTIES
set_target_properties(
	"${PROJECT_NAME}" PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF
)

# SETUP PLATFORM SPECIFIC OPTIONS
if(MSVC)
	include(MSVCConfig)
elseif(XCODE)
	include(OSXConfig)
else()
	message(FATAL_ERROR "Unsupported Platform")
endif()
target_setup_project("${PROJECT_NAME}")

# CUSTOM PROPERTIES
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(RES_SOURCE "${CMAKE_HOME_DIRECTORY}/res/")
set(RES_DEST "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG}/res")

# CUSTOM COMMANDS
add_custom_command(
	TARGET "${PROJECT_NAME}" POST_BUILD
 	COMMAND "${CMAKE_COMMAND}" -E copy_directory "${RES_SOURCE}" "${RES_DEST}"
 	DEPENDS "${RES_DEST}"
 	COMMENT "symbolic link resources folder from ${RES_SOURCE} => ${RES_DEST}"
 )

# DEBUG PRINT VARIABLES
if ("${PRINT_VARIABLES}")
	get_cmake_property(VARIABLE_NAMES VARIABLES)
	list (SORT VARIABLE_NAMES)
	foreach (VARIABLE_NAME ${VARIABLE_NAMES})
    	message(STATUS "${VARIABLE_NAME}=${${VARIABLE_NAME}}")
	endforeach()

	execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")
endif()