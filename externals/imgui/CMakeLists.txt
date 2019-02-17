project(imgui)

find_package(GLEW REQUIRED)

include_directories(
	${GLEW_INCLUDE_DIRS}
	include
)

file(GLOB SOURCES
	include/*.h
	src/*.cpp
)

add_library(imgui ${SOURCES})
target_link_libraries(imgui ${GLEW_LIBRARIES} glfw)

target_include_directories(imgui PUBLIC include)
