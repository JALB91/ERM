erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM System lib"
	LANGUAGES CXX
	LINK_PRIVATE
		ERM_Utils_static
		ERM_FileSystem_shared
		ERM_Math_static
		ERM_Log_static
)
