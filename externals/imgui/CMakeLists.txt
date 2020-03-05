project(imgui)

file(GLOB_RECURSE SOURCES
	include/*.h
	src/*.cpp
)

add_library(imgui ${SOURCES})

target_include_directories(
	"${PROJECT_NAME}"
	PUBLIC 
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE 
		IMGUI_IMPL_OPENGL_LOADER_GLEW
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		libglew_static
		glfw
)