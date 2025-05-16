function(nn_post_setup_callback)
	set(NNSourcesIncludes "")
	
	foreach(ERM_MODULE IN LISTS ERM_MODULES)
		erm_get_module_property(MODULE_BASE_DIR ${ERM_MODULE} BASE_DIR)

		# Gather nn source files
		file(GLOB_RECURSE NN_SOURCES
			"${MODULE_BASE_DIR}/NN_Sources/common/*.h"
			"${MODULE_BASE_DIR}/NN_Sources/common/*.hpp"
			"${MODULE_BASE_DIR}/NN_Sources/common/*.cpp"
			"${MODULE_BASE_DIR}/NN_Sources/${ERM_RENDERING_API}/*.h"
			"${MODULE_BASE_DIR}/NN_Sources/${ERM_RENDERING_API}/*.hpp"
			"${MODULE_BASE_DIR}/NN_Sources/${ERM_RENDERING_API}/*.cpp"
		)

		if(NN_SOURCES)
			erm_add_to_module_property(
				TARGET ERM_NN
				PROPERTY PUBLIC_INCLUDE_DIRS
				VALUES
					"${MODULE_BASE_DIR}/NN_Sources/common"
					"${MODULE_BASE_DIR}/NN_Sources/${ERM_RENDERING_API}"	
			)
		endif()

		set(NN_CPP_HEADERS "")
		set(NN_CPP_SOURCES "")

		foreach(NN_SOURCE IN LISTS NN_SOURCES)
			cmake_path(GET NN_SOURCE EXTENSION LAST_ONLY FILE_EXTENSION)
			if(FILE_EXTENSION STREQUAL ".h" OR FILE_EXTENSION STREQUAL ".hpp")
				erm_get_include_path(${NN_SOURCE} INCLUDE_PATH)
				string(APPEND NNSourcesIncludes "#include \"${INCLUDE_PATH}\"\n")
				list(APPEND NN_CPP_HEADERS ${NN_SOURCE})
			elseif(FILE_EXTENSION STREQUAL ".cpp")
				list(APPEND NN_CPP_SOURCES ${NN_SOURCE})
			endif()
		endforeach()

		erm_add_to_module_property(
			TARGET ERM_NN
			PROPERTY CPP_HEADERS
			VALUES ${NN_CPP_HEADERS}
		)
		
		erm_add_to_module_property(
			TARGET ERM_NN
			PROPERTY CPP_SOURCES
			VALUES ${NN_CPP_SOURCES}
		)
		
		# Gather nn data files
		file(GLOB_RECURSE NN_DATA
			"${MODULE_BASE_DIR}/NN_Data/common/*.nn"
			"${MODULE_BASE_DIR}/NN_Data/${ERM_RENDERING_API}/*.nn"
		)
		erm_add_to_module_property(
			TARGET ERM_NN
			PROPERTY NN_DATA
			VALUES ${NN_DATA}
		)
		
		erm_add_to_module_property(
			TARGET ERM_NN
			PROPERTY ALL_SOURCES
			VALUES ${NN_DATA}
		)

		erm_get_module_property(MODULE_GENERATED_DIR ${ERM_MODULE} GENERATED_DIR)
		
		foreach(FILE IN LISTS NN_DATA)
			string(REGEX REPLACE ".*/NN_Data/" "${MODULE_GENERATED_DIR}/" NN_OUTPUT_DIR "${FILE}")
			get_filename_component(NN_OUTPUT_DIR "${NN_OUTPUT_DIR}" DIRECTORY)
			get_filename_component(NN_OUTPUT_FILE_NAME "${FILE}" NAME_WE)
			
			set(NN_OUTPUT_HEADER "${NN_OUTPUT_DIR}/${NN_OUTPUT_FILE_NAME}.h")
			set(NN_OUTPUT_SOURCE "${NN_OUTPUT_DIR}/${NN_OUTPUT_FILE_NAME}.cpp")
			
			erm_add_to_module_property(
				TARGET ${ERM_MODULE}
				PROPERTY GENERATED_CPP_HEADERS
				VALUES ${NN_OUTPUT_HEADER}
			)

			erm_add_to_module_property(
				TARGET ${ERM_MODULE}
				PROPERTY GENERATED_CPP_SOURCES
				VALUES ${NN_OUTPUT_SOURCE}
			)

			set(NN_ARGS ${FILE} ${NN_OUTPUT_DIR})

			add_custom_command(
				OUTPUT ${NN_OUTPUT_HEADER} ${NN_OUTPUT_SOURCE}
				COMMAND $<TARGET_FILE:ERM_NN_exe>
				ARGS ${NN_ARGS}
				DEPENDS ${FILE}
			)
		endforeach()

		if(NN_DATA)
			erm_add_to_module_property(
				TARGET ${ERM_MODULE}
				PROPERTY DEPENDENCIES
				VALUES ERM_NN_exe
			)
		endif()
	endforeach()

	erm_get_module_property(ERM_NN_GENERATED_DIR ERM_NN GENERATED_DIR)
	erm_get_module_property(ERM_NN_BASE_DIR ERM_NN BASE_DIR)

	set(ERM_NN_ALL_STATEMENT_HANDLERS_FILE "${ERM_NN_GENERATED_DIR}/common/erm/nn/NNAllStatementHandlers.h")
	configure_file(
		"${ERM_NN_BASE_DIR}/Templates/NNAllStatementHandlers.h"
		"${ERM_NN_ALL_STATEMENT_HANDLERS_FILE}"
		@ONLY
	)
	erm_add_to_module_property(
		TARGET ERM_NN
		PROPERTY GENERATED_CPP_HEADERS
		VALUES "${ERM_NN_ALL_STATEMENT_HANDLERS_FILE}"
	)
endfunction()