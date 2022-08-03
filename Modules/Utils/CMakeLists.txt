# Cmake version
cmake_minimum_required(VERSION 3.15...3.17)

# Project declaration
project(
	"ERM_Utils"
	VERSION 0.0.1
	DESCRIPTION "ERM Utils module"
	LANGUAGES CXX
)

erm_setup_module_library()

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC
		$<$<BOOL:${ERM_TRACY_ENABLED}>:Tracy::TracyClient>
)
