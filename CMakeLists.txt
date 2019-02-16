cmake_minimum_required(VERSION 3.1)
project(erm)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL)
find_package(GLFW3)

add_compile_definitions(GL_SILENCE_DEPRECATION)

include_directories(
	/usr/local/Cellar/glfw/3.2.1/include/
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

add_executable(erm ${SOURCES})
target_link_libraries(
	erm
	${OPENGL_gl_LIBRARY}
	/usr/local/Cellar/glfw/3.2.1/lib/libglfw.3.2.dylib
)
