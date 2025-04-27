project(
	"ERM modules"
	VERSION 1.0.0
	DESCRIPTION "ERM modules"
)

# Updates cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include erm modules and utils
include(ERMModulesConfig)
include(ERMModulesUtils)
include(ERMModules)

set(ERM_MODULES
	ERM_ArgsParser
	ERM_Assets
	ERM_Audio
	ERM_ECS
	ERM_Editor
	ERM_Engine
	ERM_FileSystem
	ERM_Input
	ERM_Log
	ERM_Math
	ERM_Modules
	ERM_NN
	ERM_Rendering
	ERM_Setup
	ERM_STL
	ERM_System
	ERM_TypeID
	ERM_Utils
	ERM_Window
)

# Add ERM modules subdirectories
foreach(ERM_MODULE ${ERM_MODULES})
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${ERM_MODULE}")
endforeach()

foreach(ERM_MODULE ${ERM_MODULES})
	# Add common libraries to link
	erm_add_to_module_property(
		TARGET ${ERM_MODULE}
		PROPERTY LINK_PUBLIC
		VALUES
			refl-cpp
			magic_enum
	)

	# Call post setup callback if any
	erm_get_module_property(MODULE_POST_SETUP_CALLBACK ${ERM_MODULE} POST_SETUP_CALLBACK)
	if(MODULE_POST_SETUP_CALLBACK)
		message(TRACE "Post setup callback ${ERM_MODULE}")
		erm_call_function(FN_NAME "${MODULE_POST_SETUP_CALLBACK}")
	endif()
endforeach()

foreach(ERM_MODULE ${ERM_MODULES})
	erm_get_module_property(MODULE_LINK_INTERFACE ${ERM_MODULE} LINK_INTERFACE)
	erm_get_module_property(MODULE_LINK_PRIVATE ${ERM_MODULE} LINK_PRIVATE)
	erm_get_module_property(MODULE_LINK_PUBLIC ${ERM_MODULE} LINK_PUBLIC)

	erm_add_to_module_property(
		TARGET ${ERM_MODULE}
		PROPERTY ALL_DEPS
		VALUES
			${MODULE_LINK_INTERFACE}
			${MODULE_LINK_PRIVATE}
			${MODULE_LINK_PUBLIC}
	)

	# Gather all dependencies
	list(APPEND ALL_DEPS
		${MODULE_LINK_INTERFACE}
		${MODULE_LINK_PRIVATE}
		${MODULE_LINK_PUBLIC}
	)
endforeach()

foreach(ERM_MODULE ${ERM_MODULES})
	# Call pre generate callback if any
	erm_get_module_property(MODULE_PRE_GENERATE_CALLBACK ${ERM_MODULE} PRE_GENERATE_CALLBACK)
	if(MODULE_PRE_GENERATE_CALLBACK)
		message(TRACE "Pre generate callback ${ERM_MODULE}")
		erm_call_function(FN_NAME "${MODULE_PRE_GENERATE_CALLBACK}")
	endif()
endforeach()

foreach(ERM_MODULE ${ERM_MODULES})
	message(TRACE "Generating ${ERM_MODULE}")

	erm_get_module_property(MODULE_GENERATED_DIR ${ERM_MODULE} GENERATED_DIR)

	# Setup dummy cpp source
	set(DUMMY_CPP_SOURCE "${MODULE_GENERATED_DIR}/common/erm/dummy.cpp")
	erm_add_to_module_property(
		TARGET ${ERM_MODULE}
		PROPERTY DUMMY_CPP_SOURCE
		VALUES "${DUMMY_CPP_SOURCE}"
	)
	if(NOT EXISTS "${DUMMY_CPP_SOURCE}")
		file(WRITE "${DUMMY_CPP_SOURCE}" "${ERM_GENERATED_FILE_WARNING}")
	endif()

	# Gather all sources
	erm_get_module_property(MODULE_GENERATED_CPP_MODULES ${ERM_MODULE} GENERATED_CPP_MODULES)
	erm_get_module_property(MODULE_GENERATED_CPP_SOURCES ${ERM_MODULE} GENERATED_CPP_SOURCES)
	erm_get_module_property(MODULE_GENERATED_CPP_HEADERS ${ERM_MODULE} GENERATED_CPP_HEADERS)
	erm_get_module_property(MODULE_CPP_MODULES ${ERM_MODULE} CPP_MODULES)
	erm_get_module_property(MODULE_CPP_SOURCES ${ERM_MODULE} CPP_SOURCES)
	erm_get_module_property(MODULE_CPP_HEADERS ${ERM_MODULE} CPP_HEADERS)

	set(MODULE_ALL_SOURCES
		${MODULE_GENERATED_CPP_MODULES}
		${MODULE_GENERATED_CPP_SOURCES}
		${MODULE_GENERATED_CPP_HEADERS}
		${MODULE_CPP_MODULES}
		${MODULE_CPP_SOURCES}
		${MODULE_CPP_HEADERS}
	)

	erm_add_to_module_property(
		TARGET ${ERM_MODULE}
		PROPERTY ALL_SOURCES
		VALUES ${MODULE_ALL_SOURCES}
	)
	
	# Source groups
	foreach(FILE ${MODULE_ALL_SOURCES})
		erm_source_group_for_file("${FILE}")
	endforeach()

	erm_get_module_property(MODULE_EXECUTABLE ${ERM_MODULE} EXECUTABLE)
	
	if(MODULE_EXECUTABLE)
		erm_get_module_property(MODULE_MAIN_PATH ${ERM_MODULE} MODULE_MAIN_PATH)
		erm_source_group_for_file("${MODULE_MAIN_PATH}")
	endif()
	
	# Determine multi target requirements
	list(FIND ALL_DEPS "${ERM_MODULE}_obj" OBJECT)
	list(FIND ALL_DEPS "${ERM_MODULE}_static" STATIC)
	list(FIND ALL_DEPS "${ERM_MODULE}_shared" SHARED)

	if(${OBJECT} GREATER_EQUAL 0)
		set(MODULE_OBJECT ON)
	else()
		set(MODULE_OBJECT OFF)
	endif()

	if(${STATIC} GREATER_EQUAL 0)
		set(MODULE_STATIC ON)
	else()
		set(MODULE_STATIC OFF)
	endif()

	if(${SHARED} GREATER_EQUAL 0)
		set(MODULE_SHARED ON)
	else()
		set(MODULE_SHARED OFF)
	endif()

	if(NOT MODULE_OBJECT)
		if(MODULE_STATIC AND MODULE_SHARED)
			set(MODULE_OBJECT ON)
		elseif(MODULE_STATIC AND MODULE_EXECUTABLE)
			set(MODULE_OBJECT ON)
		elseif(MODULE_SHARED AND MODULE_EXECUTABLE)
			set(MODULE_OBJECT ON)
		endif()
	endif()

	set_target_properties(
		${ERM_MODULE}
		PROPERTIES
			INTERFACE TRUE
			OBJECT ${MODULE_OBJECT}
			STATIC ${MODULE_STATIC}
			SHARED ${MODULE_SHARED}
	)

	# Always create interface lib
	_erm_setup_interface_lib(${ERM_MODULE})

	# Create object lib
	if(MODULE_OBJECT)
		_erm_setup_object_lib(${ERM_MODULE})
	endif()

	# Create static lib
	if(MODULE_STATIC)
		if(MODULE_OBJECT)
			_erm_setup_static_lib(
				${ERM_MODULE} 
				"$<TARGET_OBJECTS:${ERM_MODULE}_obj>;${DUMMY_CPP_SOURCE}"
			)
		else()
			_erm_setup_static_lib(${ERM_MODULE} ${MODULE_ALL_SOURCES})
			_erm_target_source_files(${ERM_MODULE} ${ERM_MODULE}_static)
		endif()
	endif()
	
	# Create shared lib
	if(MODULE_SHARED)
		if(MODULE_OBJECT)
			_erm_setup_shared_lib(
				${ERM_MODULE} 
				"$<TARGET_OBJECTS:${ERM_MODULE}_obj>;${DUMMY_CPP_SOURCE}"
			)
		else()
			_erm_setup_shared_lib(${ERM_MODULE} ${MODULE_ALL_SOURCES})
			_erm_target_source_files(${ERM_MODULE} ${ERM_MODULE}_shared)
		endif()
	endif()

	# Create executables
	if(MODULE_EXECUTABLE)
		if(MODULE_OBJECT)
			_erm_setup_executable(
				ERM_MODULE ${ERM_MODULE}
				SOURCES $<TARGET_OBJECTS:${ERM_MODULE}_obj>
			)
		else()
			_erm_setup_executable(
				ERM_MODULE ${ERM_MODULE}
				SOURCES ${MODULE_ALL_SOURCES}
			)
		endif()
	endif()
endforeach()

add_custom_target(
	ERM_UPDATE_BINARIES
	# "$<$<CONFIG:Release>:${CMAKE_COMMAND};-E;copy_directory;${CMAKE_BINARY_DIR}/ERM/bin/Release;${ERM_ROOT_DIR}/bin>"
	COMMAND ${CMAKE_COMMAND} --build ./ --config Release
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/ERM/bin/Release" "${ERM_ROOT_DIR}/bin/${ERM_HOST_PLATFORM}"
	COMMAND_EXPAND_LISTS
	COMMENT "Updates ERM binaries"
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

foreach(ERM_MODULE ${ERM_MODULES})
	# Call post generate callback if any
	erm_get_module_property(MODULE_POST_GENERATE_CALLBACK ${ERM_MODULE} POST_GENERATE_CALLBACK)
	if(MODULE_POST_GENERATE_CALLBACK)
		message(TRACE "Post generate callback ${ERM_MODULE}")
		erm_call_function(FN_NAME "${MODULE_POST_GENERATE_CALLBACK}")
	endif()
endforeach()
