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
	$<$<BOOL:${ERM_OPEN_GL}>:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.h>
	$<$<BOOL:${ERM_OPEN_GL}>:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp>
	$<$<BOOL:${ERM_VULKAN}>:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.h>
	$<$<BOOL:${ERM_VULKAN}>:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp>
)

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${ERM_TARGET_API_INCLUDE_DIR}"
	PUBLIC 
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui/"
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/"
)

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE 
		$<$<BOOL:${ERM_OPEN_GL}>:IMGUI_IMPL_OPENGL_LOADER_GLEW>
		$<$<BOOL:${ERM_OPEN_GL}>:GLEW_STATIC>
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		$<$<BOOL:${ERM_OPEN_GL}>:${GLEW_LIBRARY}>
		glfw
)
