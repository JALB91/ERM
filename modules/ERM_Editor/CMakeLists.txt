erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Editor lib"
	LANGUAGES CXX
	EXECUTABLE
	PRIVATE_COMPILE_DEF
		ERM_ZONE_COLOR=0x111000
		$<$<BOOL:${ERM_VULKAN}>:IMGUI_DISABLE_OBSOLETE_FUNCTIONS>
	LINK_PRIVATE
		imgui
	LINK_PUBLIC
		ERM_Engine_shared
		ERM_Utils_static
		ERM_Math_static
		ERM_Rendering_shared
		ERM_ECS_shared
		ERM_FileSystem_shared
		ERM_Window_shared
		ERM_Audio_shared
		ERM_Assets_shared
		ERM_Input
		glfw
)
