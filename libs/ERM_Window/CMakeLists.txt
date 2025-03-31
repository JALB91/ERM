erm_lib_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Window lib"
	LANGUAGES CXX
	STATIC
	PRIVATE_DEPS
		$<$<BOOL:${ERM_OPEN_GL}>:libglew_static>
		glfw
		imgui
		ERM_Utils_static
	PUBLIC_DEPS
		ERM_Math_static
		ERM_Input_static
)

target_compile_definitions(
	${PROJECT_NAME}_static
	PRIVATE
		ERM_ZONE_COLOR=0x110111
		$<$<BOOL:${ERM_OPEN_GL}>:GLEW_STATIC>
)
