cmake_minimum_required(VERSION 3.1)
project(erm)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL)
find_package(GLUT)
find_package(GLEW)

add_compile_definitions(GL_SILENCE_DEPRECATION)

include_directories(
	/usr/local/Cellar/glew/2.1.0/include/
	include
)

file(GLOB SOURCES include/**.h src/**.cpp)

add_executable(erm ${SOURCES})
target_link_libraries(
	erm
	${OPENGL_gl_LIBRARY}
	${GLUT_LIBRARIES}
	${GLEW_LIBRARIES}
)
