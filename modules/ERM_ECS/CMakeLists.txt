erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM ECS lib"
	LANGUAGES CXX
	LINK_PRIVATE
		ERM_Input
		ERM_Utils_static
		ERM_Assets_shared
		ERM_Rendering_shared
	LINK_PUBLIC
		ERM_Math_static
)
