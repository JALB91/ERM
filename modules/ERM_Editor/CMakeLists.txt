erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Editor lib"
	LANGUAGES CXX
	PRIVATE_COMPILE_DEF
		ERM_ZONE_COLOR=0x111000
		$<$<BOOL:${ERM_VULKAN}>:IMGUI_DISABLE_OBSOLETE_FUNCTIONS>
	LINK_PRIVATE
		imgui
	LINK_PUBLIC
		ERM_Engine_static
		ERM_Utils_static
		ERM_Math_static
		ERM_Rendering_static
		ERM_ECS_static
		ERM_FileSystem_static
		ERM_Window_static
		ERM_Audio_static
		ERM_Assets_static
		ERM_Input_static
		glfw
)
