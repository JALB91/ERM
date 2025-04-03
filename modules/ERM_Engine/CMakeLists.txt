erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Engine lib"
	LANGUAGES CXX
	LINK_PRIVATE
		${ERM_RENDERING_API_LIB}
		glfw
		ERM_Math_static
		ERM_Audio_static
		ERM_Assets_static
		ERM_ECS_static
	LINK_PUBLIC
		ERM_Input_static
		ERM_Utils_static
		ERM_Rendering_static
		ERM_Window_static
)
