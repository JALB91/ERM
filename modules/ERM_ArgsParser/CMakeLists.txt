erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM ArgsParser lib"
	LANGUAGES CXX
	STANDALONE
	LINK_PRIVATE
		ERM_FileSystem_shared
		ERM_Utils_static
		ERM_Math_static
		ERM_Log_static
		ERM_STL
)
