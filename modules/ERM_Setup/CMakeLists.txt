erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Setup lib"
	LANGUAGES CXX
	LINK_PRIVATE
		ERM_FileSystem_static
		ERM_Log_static
		ERM_Utils_static
	LINK_PUBLIC
		ERM_ArgsParser_static
)
