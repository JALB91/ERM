erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM ArgsParser lib"
	LANGUAGES CXX
	LINK_PRIVATE
		ERM_FileSystem_static
	LINK_PUBLIC
		ERM_Utils_static
		ERM_Math_static
		ERM_Log_static
		magic_enum
)
