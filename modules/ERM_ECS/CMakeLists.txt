erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM ECS lib"
	LANGUAGES CXX
	LINK_PRIVATE
		ERM_Input_static
		ERM_Utils_static
		ERM_Assets_static
		ERM_Rendering_static
	LINK_PUBLIC
		ERM_Math_static
)
