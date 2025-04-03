erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Assets lib"
	LANGUAGES CXX
	PRIVATE_DEPS
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:assimp>
		stb_image
		tinyxml2
		fbx_sdk
		ERM_Utils_static
	PUBLIC_DEPS
		ERM_Math_static
)
