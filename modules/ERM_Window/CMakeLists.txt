erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Window lib"
	LANGUAGES CXX
	PRIVATE_COMPILE_DEF
		ERM_ZONE_COLOR=0x110111
		$<$<BOOL:${ERM_OPEN_GL}>:GLEW_STATIC>
	LINK_PRIVATE
		$<$<BOOL:${ERM_OPEN_GL}>:libglew_static>
		glfw
		imgui
		ERM_STL
		ERM_Utils_static
		ERM_Math_static
		ERM_Input
		ERM_Log_static
)
