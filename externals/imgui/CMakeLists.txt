project(imgui)

include_directories(
	include
)

file(GLOB SOURCES
	include/*.h
	src/*.cpp
)

add_library(imgui ${SOURCES})
target_link_libraries(imgui libglew_static glfw)

target_include_directories(imgui PUBLIC include)
