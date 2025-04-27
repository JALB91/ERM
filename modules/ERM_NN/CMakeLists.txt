list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(NNPostSetupCallback)

erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM NN lib"
	POST_SETUP_CALLBACK nn_post_setup_callback
	LANGUAGES CXX
	EXECUTABLE
	LINK_PUBLIC
		glm
		nlohmann_json
		ERM_Utils_static
		ERM_Math_static
		ERM_FileSystem_shared
		ERM_ArgsParser_shared
		ERM_System_shared
)
