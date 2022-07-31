# Cmake version
cmake_minimum_required(VERSION 3.15...3.17)

# Project declaration
project(
	"ERM_Input"
	VERSION 0.0.1
	DESCRIPTION "ERM Input module"
	LANGUAGES CXX
)

erm_setup_module_library()

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC 
		glfw
)