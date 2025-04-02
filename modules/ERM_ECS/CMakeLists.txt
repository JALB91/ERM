erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM ECS lib"
	LANGUAGES CXX
	STATIC
	PRIVATE_DEPS
		ERM_Input_static
		ERM_Utils_static
		ERM_Assets_static
		ERM_Rendering_static
	PUBLIC_DEPS
		ERM_Math_static
)
