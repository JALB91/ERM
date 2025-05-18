function(_erm_target_source_files ERM_MODULE TARGET_NAME)
	erm_get_module_property(MODULE_GENERATED_CPP_SOURCES ${ERM_MODULE} GENERATED_CPP_SOURCES)
	erm_get_module_property(MODULE_CPP_SOURCES ${ERM_MODULE} CPP_SOURCES)
	erm_get_module_property(MODULE_BASE_DIR ${ERM_MODULE} BASE_DIR)
	erm_get_module_property(MODULE_GENERATED_DIR ${ERM_MODULE} GENERATED_DIR)

	target_sources(
		${TARGET_NAME}
		PRIVATE
			"${MODULE_GENERATED_CPP_SOURCES}"
			"${MODULE_CPP_SOURCES}"
	)

	if(ERM_USE_CPP_MODULES)
		erm_get_module_property(MODULE_GENERATED_CPP_MODULES ${ERM_MODULE} GENERATED_CPP_MODULES)
		erm_get_module_property(MODULE_CPP_MODULES ${ERM_MODULE} CPP_MODULES)
		target_sources(
			${TARGET_NAME}
			PRIVATE
				FILE_SET CXX_MODULES
				BASE_DIRS
					"${MODULE_BASE_DIR}/Sources/common"
					"${MODULE_BASE_DIR}/Sources/${ERM_RENDERING_API}"
					"${MODULE_GENERATED_DIR}/common"
					"${MODULE_GENERATED_DIR}/${ERM_RENDERING_API}"
				FILES
					"${MODULE_GENERATED_CPP_MODULES}"
					"${MODULE_CPP_MODULES}"
		)
	endif()
endfunction()

function(_erm_target_header_files ERM_MODULE TARGET_NAME SCOPE)
	erm_get_module_property(MODULE_CPP_HEADERS ${ERM_MODULE} CPP_HEADERS)
	if(MODULE_CPP_HEADERS)
		erm_get_module_property(MODULE_BASE_DIR ${ERM_MODULE} BASE_DIR)
		target_sources(
			${TARGET_NAME}
			${SCOPE}
				FILE_SET HEADERS
				BASE_DIRS
					"${MODULE_BASE_DIR}/Sources/common"
					"${MODULE_BASE_DIR}/Sources/${ERM_RENDERING_API}"
					"${MODULE_BASE_DIR}/NN_Sources/common"
					"${MODULE_BASE_DIR}/NN_Sources/${ERM_RENDERING_API}"
					"${MODULE_BASE_DIR}/NN_Data/common"
					"${MODULE_BASE_DIR}/NN_Data/${ERM_RENDERING_API}"
				FILES
					"${MODULE_CPP_HEADERS}"
		)
	endif()

	erm_get_module_property(MODULE_GENERATED_CPP_HEADERS ${ERM_MODULE} GENERATED_CPP_HEADERS)
	if(MODULE_GENERATED_CPP_HEADERS)
		erm_get_module_property(MODULE_GENERATED_DIR ${ERM_MODULE} GENERATED_DIR)
		target_sources(
			${TARGET_NAME}
			${SCOPE}
				FILE_SET HEADERS
				BASE_DIRS
					"${MODULE_GENERATED_DIR}/common"
					"${MODULE_GENERATED_DIR}/${ERM_RENDERING_API}"
				FILES
					"${MODULE_GENERATED_CPP_HEADERS}"
		)
	endif()
endfunction()

function(_erm_target_sources ERM_MODULE TARGET_NAME)
	erm_get_module_property(MODULE_INTERFACE_INCLUDE_DIRS ${ERM_MODULE} INTERFACE_INCLUDE_DIRS)
	if(MODULE_INTERFACE_INCLUDE_DIRS)
		target_sources(
			${TARGET_NAME}
			INTERFACE
				FILE_SET HEADERS
				BASE_DIRS ${MODULE_INTERFACE_INCLUDE_DIRS}
		)
	endif()

	erm_get_module_property(MODULE_PRIVATE_INCLUDE_DIRS ${ERM_MODULE} PRIVATE_INCLUDE_DIRS)
	if(MODULE_PRIVATE_INCLUDE_DIRS)
		target_sources(
			${TARGET_NAME}
			PRIVATE
				FILE_SET HEADERS
				BASE_DIRS ${MODULE_PRIVATE_INCLUDE_DIRS}
		)
	endif()

	erm_get_module_property(MODULE_PUBLIC_INCLUDE_DIRS ${ERM_MODULE} PUBLIC_INCLUDE_DIRS)
	if(MODULE_PUBLIC_INCLUDE_DIRS)
		target_sources(
			${TARGET_NAME}
			PUBLIC
				FILE_SET HEADERS
				BASE_DIRS ${MODULE_PUBLIC_INCLUDE_DIRS}
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
	erm_get_module_property(MODULE_INTERFACE_COMPILE_DEF ${ERM_MODULE} INTERFACE_COMPILE_DEF)
	erm_get_module_property(MODULE_PRIVATE_COMPILE_DEF ${ERM_MODULE} PRIVATE_COMPILE_DEF)
	erm_get_module_property(MODULE_PUBLIC_COMPILE_DEF ${ERM_MODULE} PUBLIC_COMPILE_DEF)
	target_compile_definitions(
		${TARGET_NAME}
		INTERFACE ${MODULE_INTERFACE_COMPILE_DEF}
		PRIVATE ${MODULE_PRIVATE_COMPILE_DEF}
		PUBLIC ${MODULE_PUBLIC_COMPILE_DEF}
	)
endfunction()

function(_erm_target_link_libraries ERM_MODULE TARGET_NAME)
	erm_get_module_property(MODULE_LINK_INTERFACE ${ERM_MODULE} LINK_INTERFACE)
	erm_get_module_property(MODULE_LINK_PRIVATE ${ERM_MODULE} LINK_PRIVATE)
	erm_get_module_property(MODULE_LINK_PUBLIC ${ERM_MODULE} LINK_PUBLIC)
	target_link_libraries(
		${TARGET_NAME}
		INTERFACE
			${MODULE_LINK_INTERFACE}
		PRIVATE
			${MODULE_LINK_PRIVATE}
		PUBLIC
			${MODULE_LINK_PUBLIC}
			"${ERM_MODULE}"
	)
	message(TRACE "\tLinking with:\n\t\t${MODULE_LINK_INTERFACE}\n\t\t${MODULE_LINK_PRIVATE}\n\t\t${MODULE_LINK_PUBLIC}")
endfunction()

function(_erm_target_setup_install TARGET_NAME)
	install(
		TARGETS ${TARGET_NAME} 
		COMPONENT ERM
		RUNTIME DESTINATION "bin/${ERM_HOST_PLATFORM}"
		LIBRARY DESTINATION "lib/${ERM_HOST_PLATFORM}"
		CONFIGURATIONS Release
	)
endfunction()

function(_erm_setup_interface_lib ERM_MODULE)
	_erm_target_header_files(${ERM_MODULE} ${ERM_MODULE} INTERFACE)
	_erm_target_common_compile_definitions(${ERM_MODULE} INTERFACE)

	erm_get_module_property(MODULE_INTERFACE_INCLUDE_DIRS ${ERM_MODULE} INTERFACE_INCLUDE_DIRS)
	if(MODULE_INTERFACE_INCLUDE_DIRS)
		message(TRACE "\tIncluding: ${MODULE_INTERFACE_INCLUDE_DIRS}")
		target_sources(
			${ERM_MODULE}
			INTERFACE
				FILE_SET HEADERS
				BASE_DIRS "${MODULE_INTERFACE_INCLUDE_DIRS}"
		)
	endif()

	erm_get_module_property(MODULE_PUBLIC_INCLUDE_DIRS ${ERM_MODULE} PUBLIC_INCLUDE_DIRS)
	if(MODULE_PUBLIC_INCLUDE_DIRS)
		message(TRACE "\tIncluding: ${MODULE_PUBLIC_INCLUDE_DIRS}")
		target_sources(
			${ERM_MODULE}
			INTERFACE
				FILE_SET HEADERS
				BASE_DIRS "${MODULE_PUBLIC_INCLUDE_DIRS}"
		)
	endif()
	
	erm_get_module_property(MODULE_LINK_INTERFACE ${ERM_MODULE} LINK_INTERFACE)
	erm_get_module_property(MODULE_LINK_PUBLIC ${ERM_MODULE} LINK_PUBLIC)
	message(TRACE "\tLinking: ${MODULE_LINK_INTERFACE},${MODULE_LINK_PUBLIC}")
	target_link_libraries(
		${ERM_MODULE}
		INTERFACE
			${MODULE_LINK_INTERFACE}
			${MODULE_LINK_PUBLIC}
	)
	
	erm_get_module_property(MODULE_DEPENDENCIES ${ERM_MODULE} DEPENDENCIES)
	if(MODULE_DEPENDENCIES)
		add_dependencies(${ERM_MODULE} ${MODULE_DEPENDENCIES})
	endif()
endfunction()

function(_erm_setup_object_lib ERM_MODULE)
	set(TARGET_NAME ${ERM_MODULE}_obj)
	erm_get_module_property(MODULE_ALL_SOURCES ${ERM_MODULE} ALL_SOURCES)
	add_library(${TARGET_NAME} OBJECT "${MODULE_ALL_SOURCES}")
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
	_erm_target_setup_install(${TARGET_NAME})
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

	erm_get_module_property(MODULE_MAIN_PATH ${EXE_ERM_MODULE} MODULE_MAIN_PATH)
	erm_get_module_property(MODULE_BASE_DIR ${EXE_ERM_MODULE} BASE_DIR)

	add_executable(${EXE_TARGET_NAME} ${MODULE_MAIN_PATH} ${EXE_SOURCES})
	_erm_target_header_files(${EXE_ERM_MODULE} ${EXE_TARGET_NAME} PUBLIC)
	_erm_target_sources(${EXE_ERM_MODULE} ${EXE_TARGET_NAME})
	_erm_target_compile_definitions(${EXE_ERM_MODULE} ${EXE_TARGET_NAME})
	_erm_target_link_libraries(${EXE_ERM_MODULE} ${EXE_TARGET_NAME})
	_erm_target_setup_install(${EXE_TARGET_NAME})

	if(ERM_HOST_WINDOWS)
		add_custom_command(
			TARGET ${EXE_TARGET_NAME} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:${EXE_TARGET_NAME}>" "$<TARGET_RUNTIME_DLLS:${EXE_TARGET_NAME}>" 
			USES_TERMINAL 
			COMMAND_EXPAND_LISTS
		)
	endif()
endfunction()

## @brief Setup ERM module
##
## Option arguments:
##   - EXECUTABLE: Whether to build an executable for the module.
##   - STANDALONE: Standalone modules do not get de/initialization callbacks or linked with ERM_ModulesLib module.
##
## One-value arguments:
##   - VERSION: Module version number (<major>.<minor>.<patch>).
##   - DESCRIPTION: Description of the module.
##   - FOLDER: Folder to use (for IDEs).
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
	# Declare and parse options
	# ===========================================================================
	set(ERM_MODULE_OPTIONS
		EXECUTABLE
		STANDALONE
	)
	set(ERM_MODULE_ONE_VALUE_ARGS
		VERSION
		DESCRIPTION
		FOLDER
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
		ERM_MODULE
		"${ERM_MODULE_OPTIONS}"
		"${ERM_MODULE_ONE_VALUE_ARGS}"
		"${ERM_MODULE_MULTI_VALUE_ARGS}"
		${ARGN}
	)

	# ===========================================================================
	# Get dir name
	# ===========================================================================
	get_filename_component(DIR_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

	# ===========================================================================
	# Setup project
	# ===========================================================================
	project(
		${DIR_NAME}
		VERSION "${ERM_MODULE_VERSION}"
		DESCRIPTION "${ERM_MODULE_DESCRIPTION}"
		LANGUAGES "${ERM_MODULE_LANGUAGES}"
	)

	# ===========================================================================
	# Add interface library
	# ===========================================================================
	add_library(${DIR_NAME} INTERFACE)

	# ===========================================================================
	# Gather sources
	# ===========================================================================
	if(ERM_USE_CPP_MODULES)
		file(GLOB_RECURSE CPP_MODULES
			"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.cppm"
			"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_RENDERING_API}/*.cppm"
		)
	endif()

	file(GLOB_RECURSE CPP_HEADERS
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.hpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_RENDERING_API}/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_RENDERING_API}/*.hpp"
	)

	file(GLOB_RECURSE CPP_SOURCES
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_RENDERING_API}/*.cpp"
	)

	if(NOT ERM_RAY_TRACING_ENABLED)
		list(FILTER CPP_MODULES EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER CPP_HEADERS EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER CPP_SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
	endif()

	# ===========================================================================
	# Setup module generated dirs
	# ===========================================================================
	file(MAKE_DIRECTORY "${ERM_GENERATED_DIR}/${DIR_NAME}/common")
	file(MAKE_DIRECTORY "${ERM_GENERATED_DIR}/${DIR_NAME}/${ERM_RENDERING_API}")

	# ===========================================================================
	# Setup target properties
	# ===========================================================================
	set_target_properties(
		${DIR_NAME}
		PROPERTIES
			EXECUTABLE 					"${ERM_MODULE_EXECUTABLE}"
			STANDALONE 					"${ERM_MODULE_STANDALONE}"
			VERSION 					"${ERM_MODULE_VERSION}"
			VERSION_MAJOR				"${CMAKE_PROJECT_VERSION_MAJOR}"
			VERSION_MINOR				"${CMAKE_PROJECT_VERSION_MINOR}"
			VERSION_PATCH				"${CMAKE_PROJECT_VERSION_PATCH}"
			DESCRIPTION 				"${ERM_MODULE_DESCRIPTION}"
			FOLDER						"${ERM_MODULE_FOLDER}"
			POST_SETUP_CALLBACK 		"${ERM_MODULE_POST_SETUP_CALLBACK}"
			PRE_GENERATE_CALLBACK 		"${ERM_MODULE_PRE_GENERATE_CALLBACK}"
			POST_GENERATE_CALLBACK 		"${ERM_MODULE_POST_GENERATE_CALLBACK}"
			BASE_DIR 					"${CMAKE_CURRENT_SOURCE_DIR}"
			GENERATED_DIR 				"${ERM_GENERATED_DIR}/${DIR_NAME}"
			LANGUAGES 					"${ERM_MODULE_LANGUAGES}"
			INTERFACE_INCLUDE_DIRS 		"${ERM_MODULE_INTERFACE_INCLUDE_DIRS}"
			PRIVATE_INCLUDE_DIRS 		"${ERM_MODULE_PRIVATE_INCLUDE_DIRS}"
			PUBLIC_INCLUDE_DIRS 		"${ERM_MODULE_PUBLIC_INCLUDE_DIRS}"
			INTERFACE_COMPILE_DEF 		"${ERM_MODULE_INTERFACE_COMPILE_DEF}"
			PRIVATE_COMPILE_DEF 		"${ERM_MODULE_PRIVATE_COMPILE_DEF}"
			PUBLIC_COMPILE_DEF 			"${ERM_MODULE_PUBLIC_COMPILE_DEF}"
			LINK_INTERFACE 				"${ERM_MODULE_LINK_INTERFACE}"
			LINK_PRIVATE 				"${ERM_MODULE_LINK_PRIVATE}"
			LINK_PUBLIC 				"${ERM_MODULE_LINK_PUBLIC}"
			DEPENDENCIES 				"${ERM_MODULE_DEPENDENCIES}"
			CPP_MODULES 				"${CPP_MODULES}"
			CPP_SOURCES 				"${CPP_SOURCES}"
			CPP_HEADERS 				"${CPP_HEADERS}"
			GENERATED_CPP_MODULES 		""
			GENERATED_CPP_SOURCES 		""
			GENERATED_CPP_HEADERS 		""
	)
endfunction()
