erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Engine lib"
	LANGUAGES CXX
	EXECUTABLE
	LINK_PRIVATE
		${ERM_RENDERING_API_LIB}
		glfw
		ERM_Math_static
		ERM_Audio_shared
		ERM_Assets_shared
		ERM_ECS_shared
	LINK_PUBLIC
		ERM_Input
		ERM_Utils_static
		ERM_Rendering_shared
		ERM_Window_shared
		ERM_FileSystem_shared
)
