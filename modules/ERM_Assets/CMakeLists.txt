erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Assets lib"
	LANGUAGES CXX
	LINK_PRIVATE
		$<$<BOOL:${ERM_ASSIMP_ENABLED}>:assimp>
		stb_image
		tinyxml2
		# fbx_sdk
		ERM_FileSystem_shared
		ERM_Utils_static
		ERM_Math_static
		ERM_Log_static
		ERM_STL
)
