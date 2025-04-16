function(erm_setup_pipeline_script)
	if(ERM_VULKAN)
		if(ERM_HOST_WINDOWS)
			set(ERM_SHADERS_COMPILER "$ENV{VULKAN_SDK}\\Bin\\glslc.exe")
			string(REPLACE "\\" "/" ERM_SHADERS_COMPILER "${ERM_SHADERS_COMPILER}")
		elseif(ERM_HOST_OSX)
			set(ERM_SHADERS_COMPILER "glslc")
		endif()

		if(ERM_RAY_TRACING_ENABLED)
			set(ERM_SHADERS_COMPILER "${ERM_SHADERS_COMPILER} --target-spv=spv1.5 -I ${ERM_RES_DEST}/shaders/Vulkan/imports/")
		else()
			set(ERM_SHADERS_COMPILER "${ERM_SHADERS_COMPILER} --target-spv=spv1.3 -I ${ERM_RES_DEST}/shaders/Vulkan/imports/")
		endif()
	endif()

	set(ERM_PIPELINE_ARGS --res-dest ${ERM_RES_DEST} --res-src ${ERM_RES_SOURCE})

	if(ERM_SHADERS_COMPILER)
		list(APPEND ERM_PIPELINE_ARGS --shaders-compiler ${ERM_SHADERS_COMPILER})
	endif()

	if(ERM_RAY_TRACING_ENABLED)
		list(APPEND ERM_PIPELINE_ARGS --rtx-enabled True)
	endif()

	set(ERM_PIPELINE_ARGS "${ERM_PIPELINE_ARGS}" PARENT_SCOPE)
	set(ERM_PIPELINE_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/resources_pipeline.py" PARENT_SCOPE)
endfunction()

function(erm_setup_resources_pipeline)
	# add_custom_command(
	# 	TARGET "${PROJECT_NAME}"
	# 	POST_BUILD
	# 	COMMAND ${Python3_EXECUTABLE} ${ERM_PIPELINE_SCRIPT} ${ERM_PIPELINE_ARGS}
	# 	COMMENT "Resources Pipeline"
	# )
endfunction()

function(erm_gather_directories OUT_VAR)
	file(GLOB GLOBBED_FILES LIST_DIRECTORIES true *)
	foreach(FILE ${GLOBBED_FILES})
		if(IS_DIRECTORY "${FILE}")
			list(APPEND DIRECTORIES ${FILE})
		endif()
	endforeach()
	set("${OUT_VAR}" ${DIRECTORIES} PARENT_SCOPE)
endfunction()

function(erm_get_dir_name_from_path DIR_PATH)
	if(NOT IS_DIRECTORY "${DIR_PATH}")
		message(FATAL_ERROR "No such directory: ${DIR_PATH}")
	endif()
	cmake_path(GET DIR_PATH STEM DIR_NAME)
	set(DIR_NAME ${DIR_NAME} PARENT_SCOPE)
endfunction()

function(erm_call_function)
	set(OPTIONS)
	set(ONE_VALUE_ARGS FN_NAME)
	set(MULTI_VALUE_ARGS FN_ARGS)

	cmake_parse_arguments(
		CALLBACK
		"${OPTIONS}"
		"${ONE_VALUE_ARGS}"
		"${MULTI_VALUE_ARGS}"
		${ARGN}
	)

	# Check that the command exists
	if(NOT COMMAND ${CALLBACK_FN_NAME})
		message(FATAL_ERROR "Function '${CALLBACK_FN_NAME}' is not defined!")
	endif()

	# Call the function with the given arguments
	cmake_language(
		CALL ${CALLBACK_FN_NAME} 
		ARGS ${CALLBACK_FN_ARGS}
	)
endfunction()

# Cut the end of a string by the desired amount of chars
function(erm_string_substr_end INPUT NUM_CHARS OUTPUT)
    string(LENGTH "${INPUT}" STR_LEN)
    math(EXPR END_INDEX "${STR_LEN} - ${NUM_CHARS}")
    if(END_INDEX LESS 0)
        set(END_INDEX 0)
    endif()
    string(SUBSTRING "${INPUT}" 0 ${END_INDEX} RESULT)
    set(${OUTPUT} "${RESULT}" PARENT_SCOPE)
endfunction()
