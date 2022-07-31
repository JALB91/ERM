# Cmake version
cmake_minimum_required(VERSION 3.15...3.17)

# Project declaration
project(
	"ERM_Audio"
	VERSION 0.0.1
	DESCRIPTION "ERM Audio module"
	LANGUAGES CXX
)

# Gather sources
file(GLOB_RECURSE SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cpp"
)
erm_create_groups("${SOURCES}")

add_library("${PROJECT_NAME}" "${SOURCES}")

erm_target_setup_project()
erm_target_setup_common_defaults()

target_include_directories(
	"${PROJECT_NAME}"
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/"
)

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC
		fmod_core
		fmod_studio
		ERM_Utils
)