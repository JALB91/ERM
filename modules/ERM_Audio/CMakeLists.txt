erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Audio lib"
	LANGUAGES CXX
	LINK_PRIVATE
		ERM_Utils_static
		ERM_Math_static
		ERM_Log_static
	LINK_PUBLIC
		fmod_core
		fmod_studio
)
