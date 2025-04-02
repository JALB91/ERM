erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM ArgsParser lib"
	LANGUAGES CXX
	STATIC
	PRIVATE_DEPS
		ERM_FileSystem_static
	PUBLIC_DEPS
		ERM_Utils_static
		ERM_Math_static
		ERM_Log_static
		magic_enum
)
