# CMAKE VERSION
cmake_minimum_required(VERSION 3.15...3.17)

# Project declaration
project(
	"ERM_Main"
	VERSION 1.0.0
	DESCRIPTION "ERM Main module"
	LANGUAGES CXX
)

set(SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

add_executable("${PROJECT_NAME}" "${SOURCES}")

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${ERM_Engine_INCLUDE_DIRS}"
)

# Target compile features
target_compile_features(
	"${PROJECT_NAME}"
	PUBLIC 
		cxx_std_17
)

# Target properties
set_target_properties(
	"${PROJECT_NAME}" 
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)

# Link libraries
target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_Engine
)