cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME ERM)

project(${PROJECT_NAME})

message(${CMAKE_CURRENT_BINARY_DIR})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

find_package(OpenGL)

add_compile_definitions(GL_SILENCE_DEPRECATION)

add_subdirectory(externals/glfw)

include_directories(
	include
)

file(GLOB SOURCES 
	include/**.h 
	src/**.cpp
)

if (FALSE)
	get_cmake_property(_variableNames VARIABLES)
	list (SORT _variableNames)
	foreach (_variableName ${_variableNames})
    	message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()

	execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(
	${PROJECT_NAME}
	${OPENGL_gl_LIBRARY}
	glfw
)
