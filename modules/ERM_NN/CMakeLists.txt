function(nn_pre_generate_callback)
	set(NN_COMMAND "${CMAKE_INSTALL_PREFIX}/bin/$<CONFIG>/$<TARGET_FILE_NAME:ERM_NN_exe>")
	set(ALL_NN_SOURCES_HEADER "#if defined(ERM_NN_IMPL)
#error \"ERM_NN_IMPL already defined, this should not happen, did you include this file twice?\"
#endif

#define ERM_NN_IMPL

namespace erm::nn::internal {

static constexpr auto kERMNNCounterStart = __COUNTER__;

}

")

	foreach(ERM_MODULE ${ERM_MODULES})
		# Gather nn source files
		file(GLOB_RECURSE NN_SOURCES
			"${${ERM_MODULE}_BASE_DIR}/NN_Sources/common/*.h"
			"${${ERM_MODULE}_BASE_DIR}/NN_Sources/common/*.hpp"
			"${${ERM_MODULE}_BASE_DIR}/NN_Sources/common/*.cpp"
			"${${ERM_MODULE}_BASE_DIR}/NN_Sources/${ERM_RENDERING_API}/*.h"
			"${${ERM_MODULE}_BASE_DIR}/NN_Sources/${ERM_RENDERING_API}/*.hpp"
			"${${ERM_MODULE}_BASE_DIR}/NN_Sources/${ERM_RENDERING_API}/*.cpp"
		)
		set(${ERM_MODULE}_NN_SOURCES ${NN_SOURCES} CACHE STRING "" FORCE)
		list(APPEND ${ERM_MODULE}_ALL_SOURCES ${NN_SOURCES})

		foreach(NN_SOURCE ${NN_SOURCES})
			cmake_path(GET NN_SOURCE EXTENSION LAST_ONLY FILE_EXTENSION)
			if(FILE_EXTENSION STREQUAL ".h" OR FILE_EXTENSION STREQUAL ".hpp")
				string(APPEND ALL_NN_SOURCES_HEADER "#include \"${NN_SOURCE}\"\n")
				list(APPEND ERM_NN_CPP_HEADERS ${NN_SOURCE})
			elseif(FILE_EXTENSION STREQUAL ".cpp")
				list(APPEND ERM_NN_CPP_SOURCES ${NN_SOURCE})
			endif()
		endforeach()

		# Gather nn data files
		file(GLOB_RECURSE NN_DATA
			"${${ERM_MODULE}_BASE_DIR}/NN_Data/common/*.nn"
			"${${ERM_MODULE}_BASE_DIR}/NN_Data/${ERM_RENDERING_API}/*.nn"
		)
		set(${ERM_MODULE}_NN_DATA ${NN_DATA} CACHE STRING "" FORCE)
		list(APPEND ${ERM_MODULE}_ALL_SOURCES ${NN_DATA})

		set(${ERM_MODULE}_ALL_SOURCES ${${ERM_MODULE}_ALL_SOURCES} CACHE STRING "" FORCE)

		foreach(FILE ${NN_DATA})
			string(REGEX REPLACE ".*/NN_Data/" "${${ERM_MODULE}_GENERATED_DIR}/" NN_OUTPUT_DIR "${FILE}")
			get_filename_component(NN_OUTPUT_DIR "${NN_OUTPUT_DIR}" DIRECTORY)
			get_filename_component(NN_OUTPUT_FILE_NAME "${FILE}" NAME_WE)
			
			set(NN_OUTPUT_HEADER "${NN_OUTPUT_DIR}/${NN_OUTPUT_FILE_NAME}.h")
			set(NN_OUTPUT_SOURCE "${NN_OUTPUT_DIR}/${NN_OUTPUT_FILE_NAME}.cpp")

			list(APPEND ${ERM_MODULE}_GENERATED_CPP_HEADERS ${NN_OUTPUT_HEADER})
			set(${ERM_MODULE}_GENERATED_CPP_HEADERS ${${ERM_MODULE}_GENERATED_CPP_HEADERS} CACHE STRING "" FORCE)
			list(APPEND ${ERM_MODULE}_GENERATED_CPP_SOURCES ${NN_OUTPUT_SOURCE})
			set(${ERM_MODULE}_GENERATED_CPP_SOURCES ${${ERM_MODULE}_GENERATED_CPP_SOURCES} CACHE STRING "" FORCE)

			list(APPEND NN_GENERATE_ARGS ${FILE} ${NN_OUTPUT_DIR})

			add_custom_command(
				OUTPUT ${NN_OUTPUT_HEADER} ${NN_OUTPUT_SOURCE}
				COMMAND ${NN_COMMAND} ${NN_GENERATE_ARGS}
				DEPENDS ${FILE}
			)
		endforeach()
	endforeach()

	string(APPEND ALL_NN_SOURCES_HEADER "

namespace erm::nn::internal {

static constexpr auto kERMNNCounterEnd = __COUNTER__;

}

#define REGISTER_STATEMENT_HANDLERS(STATEMENT_HANDLERS_MAP) \\
utils::for_constexpr<erm::nn::internal::kERMNNCounterStart + 1, erm::nn::internal::kERMNNCounterEnd - (erm::nn::internal::kERMNNCounterStart + 1)>([&] (const auto index) { \\
	addStatementHandler<index.value>(STATEMENT_HANDLERS_MAP); \\
});

")

	file(MAKE_DIRECTORY "${ERM_GENERATED_DIR}/ERM_NN/common/erm/nn")
	file(WRITE "${ERM_GENERATED_DIR}/ERM_NN/common/erm/nn/NNAllStatementHandlers.h" "${ALL_NN_SOURCES_HEADER}")
	list(APPEND ERM_NN_GENERATED_CPP_HEADERS "${ERM_GENERATED_DIR}/ERM_NN/common/erm/nn/NNAllStatementHandlers.h")
	set(ERM_NN_GENERATED_CPP_HEADERS ${ERM_NN_GENERATED_CPP_HEADERS} CACHE STRING "" FORCE)
endfunction()

function(nn_post_generate_callback)
	foreach(ERM_MODULE ${ERM_MODULES})
		if(${ERM_MODULE}_NN_DATA)
			add_dependencies(${ERM_MODULE} ERM_NN_exe)
		endif()
	endforeach()
endfunction()


erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM NN lib"
	PRE_GENERATE_CALLBACK nn_pre_generate_callback
	POST_GENERATE_CALLBACK nn_post_generate_callback
	LANGUAGES CXX
	PUBLIC_DEPS
		glm
		nlohmann_json
		magic_enum
		refl-cpp
		ERM_Utils_static
		ERM_Math_static
		ERM_FileSystem_static
		ERM_ArgsParser_static
)
