erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Setup lib"
	LANGUAGES CXX
	EXECUTABLE
	LINK_PRIVATE
		ERM_FileSystem_shared
		ERM_Log_static
		ERM_Utils_static
		ERM_System_shared
	LINK_PUBLIC
		ERM_ArgsParser_shared
)
