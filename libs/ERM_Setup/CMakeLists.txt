erm_lib_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Setup lib"
	LANGUAGES CXX
	EXECUTABLES
	PRIVATE_DEPS
		ERM_FileSystem_static
		ERM_Log_static
		ERM_Utils_static
	PUBLIC_DEPS
		ERM_ArgsParser_static
)
