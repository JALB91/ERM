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
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
)

add_library("${PROJECT_NAME}"
	${SOURCES}
	$<$<STREQUAL:"${TARGET_API}","OpenGl">:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.h>
	$<$<STREQUAL:"${TARGET_API}","OpenGl">:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp>
	$<$<STREQUAL:"${TARGET_API}","Vulkan">:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.h>
	$<$<STREQUAL:"${TARGET_API}","Vulkan">:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp>
)

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${TARGET_API_INCLUDE_DIR}"
	PUBLIC 
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui/"
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/"
)

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE 
		$<$<STREQUAL:"${TARGET_API}","OpenGl">:IMGUI_IMPL_OPENGL_LOADER_GLEW>
		$<$<STREQUAL:"${TARGET_API}","OpenGl">:GLEW_STATIC>
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		$<$<STREQUAL:"${TARGET_API}","OpenGl">:${GLEW_LIBRARY}>
		glfw
)
