function(erm_get_include_path FILE OUT)
	string(REGEX REPLACE ".*/common/" "" FILE "${FILE}")
	string(REGEX REPLACE ".*/${ERM_RENDERING_API}/" "" FILE "${FILE}")
	set(${OUT} ${FILE} PARENT_SCOPE)
endfunction()

function(erm_source_group_for_file FILE)
	cmake_path(GET FILE PARENT_PATH PARENT_DIR)
	cmake_path(GET FILE EXTENSION LAST_ONLY FILE_EXTENSION)

	string(REGEX REPLACE ".*/common/" "" GROUP "${PARENT_DIR}")
	string(REGEX REPLACE ".*/${ERM_RENDERING_API}/" "" GROUP "${GROUP}")

	string(FIND "${FILE}" "/generated/" IS_GENERATED)
	string(FIND "${FILE}" "/NN_Sources/" IS_NN_SOURCE)

	if(IS_GENERATED GREATER_EQUAL 0)
		set(GROUP "Generated/${GROUP}")
	elseif(IS_NN_SOURCE GREATER_EQUAL 0)
		set(GROUP "NN_Sources/${GROUP}")
	elseif(FILE_EXTENSION STREQUAL ".h" OR FILE_EXTENSION STREQUAL ".hpp" OR FILE_EXTENSION STREQUAL ".cpp")
		set(GROUP "Sources/${GROUP}")
	elseif(FILE_EXTENSION STREQUAL ".nn")
		set(GROUP "NN_Data/${GROUP}")
	else()
		message(WARNING "Unknown file extension ${FILE}")
	endif()

	source_group("${GROUP}" FILES "${FILE}")
endfunction()

function(erm_get_module_property OUT_VAR MODULE PROPERTY)
	get_target_property(VALUE ${MODULE} ${PROPERTY})

	if("${VALUE}" STREQUAL "VALUE-NOTFOUND")
		set("${OUT_VAR}" "" PARENT_SCOPE)
	else()
		set("${OUT_VAR}" "${VALUE}" PARENT_SCOPE)
	endif()
endfunction()

function(erm_add_to_module_property)
	set(ERM_TARGET_OPTIONS)
	set(ERM_TARGET_ONE_VALUE_ARGS TARGET PROPERTY)
	set(ERM_TARGET_MULTI_VALUE_ARGS VALUES)

	cmake_parse_arguments(
		OPTS
		"${ERM_TARGET_OPTIONS}"
		"${ERM_TARGET_ONE_VALUE_ARGS}"
		"${ERM_TARGET_MULTI_VALUE_ARGS}"
		${ARGN}
	)

	erm_get_module_property(VALUE ${OPTS_TARGET} ${OPTS_PROPERTY})
	list(APPEND VALUE ${OPTS_VALUES})

	set_target_properties(
		${OPTS_TARGET}
		PROPERTIES
			${OPTS_PROPERTY} "${VALUE}"
	)
endfunction()


set(ERM_GENERATED_FILE_WARNING "/*
===============================================================
| 		  This file have been automatically generated  		  |
|            any local changes will be overridden			  |
===============================================================
*/")
