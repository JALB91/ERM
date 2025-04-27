list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(ModulesPostSetupCallback)
include(ModulesPreGenerateCallback)

erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Modules"
	POST_SETUP_CALLBACK modules_post_setup_callback
	PRE_GENERATE_CALLBACK modules_pre_generate_callback
	LANGUAGES CXX
	STANDALONE
	LINK_PUBLIC
		ERM_Math_static
		ERM_TypeID_static
)
