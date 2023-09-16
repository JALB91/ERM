project(
	"ERM_Window"
	VERSION 0.0.1
	DESCRIPTION "ERM Window lib"
	LANGUAGES CXX
)

erm_setup_library()

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x110111
)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		$<$<BOOL:${ERM_OPEN_GL}>:libglew_static>
		glfw
		imgui
		ERM_Utils
	PUBLIC
		ERM_Math
		ERM_Input
)
