erm_library_project(0.0.1 "ERM Window lib")

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
