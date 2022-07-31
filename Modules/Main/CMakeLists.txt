# CMAKE VERSION
cmake_minimum_required(VERSION 3.15...3.17)

# Project declaration
project(
	"ERM_Main"
	VERSION 1.0.0
	DESCRIPTION "ERM Main module"
	LANGUAGES CXX
)

erm_setup_module_executable()

# Link libraries
target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_Engine
)