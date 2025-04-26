function(_erm_target_source_files ERM_MODULE TARGET_NAME)
	target_sources(
		${TARGET_NAME}
		PRIVATE
			"${${ERM_MODULE}_GENERATED_CPP_SOURCES}"
			"${${ERM_MODULE}_CPP_SOURCES}"
	)

	if(ERM_USE_CPP_MODULES)
		target_sources(
			${TARGET_NAME}
			PRIVATE
				FILE_SET CXX_MODULES
				BASE_DIRS
					"${${ERM_MODULE}_BASE_DIR}/Sources/common"
					"${${ERM_MODULE}_BASE_DIR}/Sources/${ERM_RENDERING_API}"
					"${${ERM_MODULE}_GENERATED_DIR}/common"
					"${${ERM_MODULE}_GENERATED_DIR}/${ERM_RENDERING_API}"
				FILES
					"${${ERM_MODULE}_GENERATED_CPP_MODULES}"
					"${${ERM_MODULE}_CPP_MODULES}"
		)
	endif()
endfunction()

function(_erm_target_header_files ERM_MODULE TARGET_NAME SCOPE)
	if(${ERM_MODULE}_CPP_HEADERS)
		target_sources(
			${TARGET_NAME}
			${SCOPE}
				FILE_SET HEADERS
				BASE_DIRS
					"${${ERM_MODULE}_BASE_DIR}/Sources/common"
					"${${ERM_MODULE}_BASE_DIR}/Sources/${ERM_RENDERING_API}"
					"${${ERM_MODULE}_BASE_DIR}/NN_Sources/common"
					"${${ERM_MODULE}_BASE_DIR}/NN_Sources/${ERM_RENDERING_API}"
					"${${ERM_MODULE}_BASE_DIR}/NN_Data/common"
					"${${ERM_MODULE}_BASE_DIR}/NN_Data/${ERM_RENDERING_API}"
				FILES
					"${${ERM_MODULE}_CPP_HEADERS}"
		)
	endif()

	if(${ERM_MODULE}_GENERATED_CPP_HEADERS)
		target_sources(
			${TARGET_NAME}
			${SCOPE}
				FILE_SET HEADERS
				BASE_DIRS
					"${${ERM_MODULE}_GENERATED_DIR}/common"
					"${${ERM_MODULE}_GENERATED_DIR}/${ERM_RENDERING_API}"
				FILES
					"${${ERM_MODULE}_GENERATED_CPP_HEADERS}"
		)
	endif()
endfunction()

function(_erm_target_sources ERM_MODULE TARGET_NAME)
	if(${ERM_MODULE}_INTERFACE_INCLUDE_DIRS)
		target_sources(
			${TARGET_NAME}
			INTERFACE
				FILE_SET HEADERS
				BASE_DIRS "${${ERM_MODULE}_INTERFACE_INCLUDE_DIRS}"
		)
	endif()

	if(${ERM_MODULE}_PRIVATE_INCLUDE_DIRS)
		target_sources(
			${TARGET_NAME}
			PRIVATE
				FILE_SET HEADERS
				BASE_DIRS "${${ERM_MODULE}_PRIVATE_INCLUDE_DIRS}"
		)
	endif()

	if(${ERM_MODULE}_PUBLIC_INCLUDE_DIRS)
		target_sources(
			${TARGET_NAME}
			PUBLIC
				FILE_SET HEADERS
				BASE_DIRS "${${ERM_MODULE}_PUBLIC_INCLUDE_DIRS}"
		)
	endif()
endfunction()


function(_erm_target_common_compile_definitions TARGET_NAME SCOPE)
	target_compile_definitions(
		${TARGET_NAME}
		${SCOPE}
			$<$<BOOL:${ERM_ASSIMP_ENABLED}>:ERM_ASSIMP_ENABLED>
			$<$<BOOL:${ERM_RAY_TRACING_ENABLED}>:ERM_RAY_TRACING_ENABLED>
			$<$<BOOL:${ERM_TRACY_ENABLED}>:TRACY_ENABLE>
	)
endfunction()

function(_erm_target_compile_definitions ERM_MODULE TARGET_NAME)
	target_compile_definitions(
		${TARGET_NAME}
		INTERFACE "${${ERM_MODULE}_INTERFACE_COMPILE_DEF}"
		PRIVATE "${${ERM_MODULE}_PRIVATE_COMPILE_DEF}"
		PUBLIC "${${ERM_MODULE}_PUBLIC_COMPILE_DEF}"
	)
endfunction()

function(_erm_target_link_libraries ERM_MODULE TARGET_NAME)
	target_link_libraries(
		${TARGET_NAME}
		INTERFACE
			"${${ERM_MODULE}_LINK_INTERFACE}"
		PRIVATE
			"${${ERM_MODULE}_LINK_PRIVATE}"
		PUBLIC
			"${${ERM_MODULE}_LINK_PUBLIC}"
			"${ERM_MODULE}"
	)
	message(TRACE "\tLinking with:\n\t\t${${ERM_MODULE}_LINK_INTERFACE}\n\t\t${${ERM_MODULE}_LINK_PRIVATE}\n\t\t${${ERM_MODULE}_LINK_PUBLIC}")
endfunction()

function(_erm_setup_interface_lib ERM_MODULE)
	add_library(${ERM_MODULE} INTERFACE)
	_erm_target_header_files(${ERM_MODULE} ${ERM_MODULE} INTERFACE)
	_erm_target_common_compile_definitions(${ERM_MODULE} INTERFACE)
	if(${ERM_MODULE}_INTERFACE_INCLUDE_DIRS)
		message(TRACE "\tIncluding: ${${ERM_MODULE}_INTERFACE_INCLUDE_DIRS}")
		target_sources(
			${ERM_MODULE}
			INTERFACE
				FILE_SET HEADERS
				BASE_DIRS "${${ERM_MODULE}_INTERFACE_INCLUDE_DIRS}"
		)
	endif()
	if(${ERM_MODULE}_PUBLIC_INCLUDE_DIRS)
		message(TRACE "\tIncluding: ${${ERM_MODULE}_PUBLIC_INCLUDE_DIRS}")
		target_sources(
			${ERM_MODULE}
			INTERFACE
				FILE_SET HEADERS
				BASE_DIRS "${${ERM_MODULE}_PUBLIC_INCLUDE_DIRS}"
		)
	endif()
	
	message(TRACE "\tLinking: ${${ERM_MODULE}_LINK_INTERFACE},${${ERM_MODULE}_LINK_PUBLIC}")
	target_link_libraries(
		${ERM_MODULE}
		INTERFACE
			"${${ERM_MODULE}_LINK_INTERFACE}"
			"${${ERM_MODULE}_LINK_PUBLIC}"
	)
	
	if(${ERM_MODULE}_DEPENDENCIES)
		add_dependencies(${ERM_MODULE} ${${ERM_MODULE}_DEPENDENCIES})
	endif()
endfunction()

function(_erm_setup_object_lib ERM_MODULE)
	set(TARGET_NAME ${ERM_MODULE}_obj)
	add_library(${TARGET_NAME} OBJECT "${${ERM_MODULE}_ALL_SOURCES}")
	_erm_target_header_files(${ERM_MODULE} ${TARGET_NAME} PUBLIC)
	_erm_target_source_files(${ERM_MODULE} ${TARGET_NAME})
	_erm_target_sources(${ERM_MODULE} ${TARGET_NAME})
	_erm_target_compile_definitions(${ERM_MODULE} ${TARGET_NAME})
	_erm_target_link_libraries(${ERM_MODULE} ${TARGET_NAME})
endfunction()

function(_erm_setup_static_lib ERM_MODULE SOURCES)
	set(TARGET_NAME ${ERM_MODULE}_static)
	add_library(${TARGET_NAME} STATIC ${SOURCES})
	_erm_target_header_files(${ERM_MODULE} ${TARGET_NAME} PUBLIC)
	_erm_target_sources(${ERM_MODULE} ${TARGET_NAME})
	_erm_target_compile_definitions(${ERM_MODULE} ${TARGET_NAME})
	_erm_target_link_libraries(${ERM_MODULE} ${TARGET_NAME})
endfunction()

function(_erm_setup_shared_lib ERM_MODULE SOURCES)
	set(TARGET_NAME ${ERM_MODULE}_shared)
	add_library(${TARGET_NAME} SHARED ${SOURCES})
	_erm_target_header_files(${ERM_MODULE} ${TARGET_NAME} PUBLIC)
	_erm_target_sources(${ERM_MODULE} ${TARGET_NAME})
	_erm_target_compile_definitions(${ERM_MODULE} ${TARGET_NAME})
	_erm_target_link_libraries(${ERM_MODULE} ${TARGET_NAME})
endfunction()

function(_erm_setup_executable)
	# ===========================================================================
	# Parse options
	# ===========================================================================
	set(EXE_OPTIONS)
	set(EXE_ONE_VALUE_ARGS ERM_MODULE)
	set(EXE_MULTI_VALUE_ARGS SOURCES)
	
	cmake_parse_arguments(
		EXE
		"${EXE_OPTIONS}"
		"${EXE_ONE_VALUE_ARGS}"
		"${EXE_MULTI_VALUE_ARGS}"
		${ARGN}
	)

	set(EXE_TARGET_NAME ${EXE_ERM_MODULE}_exe)

	add_executable(${EXE_TARGET_NAME} ${${EXE_ERM_MODULE}_MODULE_MAIN_PATH} ${EXE_SOURCES})
	_erm_target_header_files(${EXE_ERM_MODULE} ${EXE_TARGET_NAME} PUBLIC)
	_erm_target_sources(${EXE_ERM_MODULE} ${EXE_TARGET_NAME})
	_erm_target_compile_definitions(${EXE_ERM_MODULE} ${EXE_TARGET_NAME})
	_erm_target_link_libraries(${EXE_ERM_MODULE} ${EXE_TARGET_NAME})

	add_custom_command(
		TARGET ${EXE_TARGET_NAME} POST_BUILD 
		COMMAND "${CMAKE_COMMAND}" -E make_directory "${${EXE_ERM_MODULE}_BASE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>"
		COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${EXE_TARGET_NAME}>" "${${EXE_ERM_MODULE}_BASE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>/${EXE_TARGET_NAME}"
		BYPRODUCTS "${${EXE_ERM_MODULE}_BASE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>/${EXE_TARGET_NAME}"
	)
	
	if(ERM_HOST_OSX)
		add_custom_command(
			TARGET ${EXE_TARGET_NAME} POST_BUILD 
			COMMAND  codesign --force --deep --sign - "${${EXE_ERM_MODULE}_BASE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>/${EXE_TARGET_NAME}"
		)
	endif()

	if(ERM_HOST_WINDOWS AND $<TARGET_RUNTIME_DLLS:${EXE_TARGET_NAME}>)
		# Copy DLLs
		add_custom_command(
			TARGET ${EXE_TARGET_NAME} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_RUNTIME_DLLS:${EXE_TARGET_NAME}>" "$<TARGET_FILE_DIR:${EXE_TARGET_NAME}>"
			COMMAND_EXPAND_LISTS
		)
	endif()
endfunction()

## @brief Setup ERM module
##
## Option arguments:
##   - EXECUTABLE: Whether to build an executable for the module.
##
## One-value arguments:
##   - VERSION: Module version number (<major>.<minor>.<patch>).
##   - DESCRIPTION: Description of the module.
##   - POST_SETUP_CALLBACK: [OPTIONAL] Callback to be triggered after modules setup.
##   - PRE_GENERATE_CALLBACK: [OPTIONAL] Callback to be triggered before modules generating.
##   - POST_GENERATE_CALLBACK: [OPTIONAL] Callback to be triggered after modules generating.
##
## Multi-value arguments:
##   - LANGUAGES: List of languages the module uses.
##   - INTERFACE_INCLUDE_DIRS: List of INTERFACE include dirs of the module.
##   - PRIVATE_INCLUDE_DIRS: List of PRIVATE include dirs of the module.
##   - PUBLIC_INCLUDE_DIRS: List of PUBLIC include dirs of the module.
##   - INTERFACE_COMPILE_DEF: List of INTERFACE compile definitions of the module.
##   - PRIVATE_COMPILE_DEF: List of PRIVATE compile definitions of the module.
##   - PUBLIC_COMPILE_DEF: List of PUBLIC compile definitions of the module.
##   - LINK_INTERFACE: List of INTERFACE to link to the module.
##   - LINK_PRIVATE: List of PRIVATE to link to the module.
##   - LINK_PUBLIC: List of PUBLIC to link to the module.
##   - DEPENDENCIES: List of dependencies.
##
## Example usage:
##   erm_project_setup_2(
##   	VERSION 1.0.0
##   	DESCRIPTION "My module description"
##   	PRE_GENERATE_CALLBACK my_module_pre_generate_callback
##   	LANGUAGES CXX
##   	LINK_PRIVATE ModuleA_static ModuleB_shared
##   )
##   
function(erm_module_setup)
	# ===========================================================================
	# Get dir name
	# ===========================================================================
	get_filename_component(DIR_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
	set(${DIR_NAME}_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "" FORCE)

	# ===========================================================================
	# Unset all
	# ===========================================================================
	unset("${DIR_NAME}_VERSION" CACHE)
	unset("${DIR_NAME}_DESCRIPTION" CACHE)
	unset("${DIR_NAME}_PRE_GENERATE_CALLBACK" CACHE)
	unset("${DIR_NAME}_POST_GENERATE_CALLBACK" CACHE)
	unset("${DIR_NAME}_LANGUAGES" CACHE)
	unset("${DIR_NAME}_INTERFACE_INCLUDE_DIRS" CACHE)
	unset("${DIR_NAME}_PRIVATE_INCLUDE_DIRS" CACHE)
	unset("${DIR_NAME}_PUBLIC_INCLUDE_DIRS" CACHE)
	unset("${DIR_NAME}_INTERFACE_COMPILE_DEF" CACHE)
	unset("${DIR_NAME}_PRIVATE_COMPILE_DEF" CACHE)
	unset("${DIR_NAME}_PUBLIC_COMPILE_DEF" CACHE)
	unset("${DIR_NAME}_LINK_INTERFACE" CACHE)
	unset("${DIR_NAME}_LINK_PRIVATE" CACHE)
	unset("${DIR_NAME}_LINK_PUBLIC" CACHE)
	unset("${DIR_NAME}_DEPENDENCIES" CACHE)

	# ===========================================================================
	# Declare and parse options
	# ===========================================================================
	set(ERM_MODULE_OPTIONS
		EXECUTABLE
	)
	set(ERM_MODULE_ONE_VALUE_ARGS
		VERSION
		DESCRIPTION
		POST_SETUP_CALLBACK
		PRE_GENERATE_CALLBACK
		POST_GENERATE_CALLBACK
	)
	set(ERM_MODULE_MULTI_VALUE_ARGS 
		LANGUAGES
		INTERFACE_INCLUDE_DIRS
		PRIVATE_INCLUDE_DIRS
		PUBLIC_INCLUDE_DIRS
		INTERFACE_COMPILE_DEF
		PRIVATE_COMPILE_DEF
		PUBLIC_COMPILE_DEF
		LINK_INTERFACE
		LINK_PRIVATE
		LINK_PUBLIC
		DEPENDENCIES
	)
	
	cmake_parse_arguments(
		${DIR_NAME}
		"${ERM_MODULE_OPTIONS}"
		"${ERM_MODULE_ONE_VALUE_ARGS}"
		"${ERM_MODULE_MULTI_VALUE_ARGS}"
		${ARGN}
	)

	# ===========================================================================
	# Cache variables
	# ===========================================================================
	set(${DIR_NAME}_EXECUTABLE "${${DIR_NAME}_EXECUTABLE}" CACHE BOOL "" FORCE)
	set(${DIR_NAME}_VERSION "${${DIR_NAME}_VERSION}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_DESCRIPTION "${${DIR_NAME}_DESCRIPTION}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_POST_SETUP_CALLBACK "${${DIR_NAME}_POST_SETUP_CALLBACK}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_PRE_GENERATE_CALLBACK "${${DIR_NAME}_PRE_GENERATE_CALLBACK}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_POST_GENERATE_CALLBACK "${${DIR_NAME}_POST_GENERATE_CALLBACK}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_LANGUAGES "${${DIR_NAME}_LANGUAGES}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_INTERFACE_INCLUDE_DIRS "${${DIR_NAME}_INTERFACE_INCLUDE_DIRS}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_PRIVATE_INCLUDE_DIRS "${${DIR_NAME}_PRIVATE_INCLUDE_DIRS}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_PUBLIC_INCLUDE_DIRS "${${DIR_NAME}_PUBLIC_INCLUDE_DIRS}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_INTERFACE_COMPILE_DEF "${${DIR_NAME}_INTERFACE_COMPILE_DEF}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_PRIVATE_COMPILE_DEF "${${DIR_NAME}_PRIVATE_COMPILE_DEF}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_PUBLIC_COMPILE_DEF "${${DIR_NAME}_PUBLIC_COMPILE_DEF}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_LINK_INTERFACE "${${DIR_NAME}_LINK_INTERFACE}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_LINK_PRIVATE "${${DIR_NAME}_LINK_PRIVATE}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_LINK_PUBLIC "${${DIR_NAME}_LINK_PUBLIC}" CACHE STRING "" FORCE)
	set(${DIR_NAME}_DEPENDENCIES "${${DIR_NAME}_DEPENDENCIES}" CACHE STRING "" FORCE)

	# ===========================================================================
	# Setup project
	# ===========================================================================
	project(
		${DIR_NAME}
		VERSION "${${DIR_NAME}_VERSION}"
		DESCRIPTION "${${DIR_NAME}_DESCRIPTION}"
		LANGUAGES "${${DIR_NAME}_LANGUAGES}"
	)

	# ===========================================================================
	# Gather sources
	# ===========================================================================
	erm_gather_sources(${CMAKE_CURRENT_SOURCE_DIR})

	# ===========================================================================
	# Add to ERM_MODULES
	# ===========================================================================
	list(APPEND ERM_MODULES ${DIR_NAME})
	set(ERM_MODULES ${ERM_MODULES} CACHE STRING "" FORCE)

	# ===========================================================================
	# Set module base dir
	# ===========================================================================
	set(${DIR_NAME}_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "" FORCE)

	# ===========================================================================
	# Setup module generated dirs
	# ===========================================================================
	set(${DIR_NAME}_GENERATED_DIR "${ERM_GENERATED_DIR}/${DIR_NAME}" CACHE PATH "" FORCE)
	file(MAKE_DIRECTORY "${${DIR_NAME}_GENERATED_DIR}/common")
	file(MAKE_DIRECTORY "${${DIR_NAME}_GENERATED_DIR}/${ERM_RENDERING_API}")
endfunction()

option(ERM_USE_CPP_MODULES "If on it uses cxx modules instead of header files" OFF)
