erm_lib_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM System lib"
	LANGUAGES CXX
	STATIC
	PRIVATE_DEPS
		ERM_Utils_static
		ERM_FileSystem_static
	PUBLIC_DEPS
		refl-cpp
		ERM_Math_static
)
