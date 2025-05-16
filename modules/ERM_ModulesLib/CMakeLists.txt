list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(ModulesLibPostSetupCallback)
include(ModulesLibPreGenerateCallback)

erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Modules lib"
	POST_SETUP_CALLBACK modules_lib_post_setup_callback
	PRE_GENERATE_CALLBACK modules_lib_pre_generate_callback
	LANGUAGES CXX
	STANDALONE
	LINK_PRIVATE
		ERM_Math_static
		ERM_Utils_static
)
