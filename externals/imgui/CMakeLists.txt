project(imgui)

file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imconfig.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_internal.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_rectpack.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_textedit.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_truetype.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_glfw.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_glfw.cpp"
)

add_library("${PROJECT_NAME}"
	${SOURCES}
	$<$<STREQUAL:"${TARGET_API}","OpenGl">:${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_opengl3.h>
	$<$<STREQUAL:"${TARGET_API}","OpenGl">:${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/imgui_impl_opengl3.cpp>
)

target_include_directories(
	"${PROJECT_NAME}"
	PUBLIC 
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui/"
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/"
)

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE 
		IMGUI_IMPL_OPENGL_LOADER_GLEW
		GLEW_STATIC
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		"${GLEW_LIBRARY}"
		glfw
)