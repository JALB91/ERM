function(modules_post_setup_callback)
	foreach(ERM_MODULE ${ERM_MODULES})
		if(NOT "${ERM_MODULE}" STREQUAL "ERM_Modules")
			list(APPEND ${ERM_MODULE}_PRIVATE_DEPS ERM_Modules_static)
			set(${ERM_MODULE}_PRIVATE_DEPS "${${ERM_MODULE}_PRIVATE_DEPS}" CACHE STRING "" FORCE)
		endif()
	endforeach()
endfunction()

function(modules_pre_generate_callback)
	# Gather modules' dependencies
	foreach(ERM_MODULE ${ERM_MODULES})
		foreach(DEP ${${ERM_MODULE}_ALL_DEPS})
			string(REPLACE "_static" "" DEP DEP)
			string(REPLACE "_shared" "" DEP DEP)
			string(REPLACE "_int" "" DEP DEP)
			string(REPLACE "_obj" "" DEP DEP)
			string(REPLACE "_exe" "" DEP DEP)

			list(FIND ERM_MODULES ${DEP} INDEX)

			if(${INDEX} GREATER_EQUAL 0)
				list(APPEND ${ERM_MODULE}_ALL_DEPS_STR ${DEP})
			endif()
		endforeach()
		list(REMOVE_DUPLICATES ${ERM_MODULE}_ALL_DEPS_STR)

		set(MODULE_SOURCE ${ERM_GENERATED_HEADER_WARNING})
		string(APPEND MODULE_SOURCE "\n\n")

		foreach(DEP ${${ERM_MODULE}_ALL_DEPS_STR})
			string(APPEND "#include <erm/${DEP}.h>\n")
		endforeach()
		

		string(APPEND MODULE_SOURCE "
#include <erm/modules/Modules.h>

namespace erm {

class ${ERM_MODULE} : public ModuleType<
")

		foreach(DEP ${${ERM_MODULE}_ALL_DEPS_STR})
			string(APPEND MODULE_SOURCE "\t${DEP},\n")
		endforeach()

		string(LENGTH MODULE_SOURCE LEN)
		math(EXPR LEN "${LEN} - 2")
		string(SUBSTRING MODULE_SOURCE 0 ${LEN} MODULE_SOURCE)
		string(APPEND MODULE_SOURCE ">
{
public:
	static void init();

};
")

		set(${ERM_MODULE}_MODULE_SOURCE "${${ERM_MODULE}_GENERATED_DIR}/common/erm/${ERM_MODULE}.cpp" CACHE FILEPATH "" FORCE)
		file(WRITE "${${ERM_MODULE}_MODULE_SOURCE}" "${MODULE_SOURCE}")
		list(APPEND ${ERM_MODULE}_GENERATED_CPP_SOURCES "${${ERM_MODULE}_MODULE_SOURCE}")
	endforeach()
endfunction()


erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Modules"
	POST_SETUP_CALLBACK modules_post_setup_callback
	PRE_GENERATE_CALLBACK modules_pre_generate_callback
	LANGUAGES CXX
	PUBLIC_DEPS
		refl-cpp
)
