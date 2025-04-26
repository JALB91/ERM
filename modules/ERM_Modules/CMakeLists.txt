function(modules_post_setup_callback)
	foreach(ERM_MODULE ${ERM_MODULES})
		if("${ERM_MODULE}" STREQUAL "ERM_Modules")
			continue()
		endif()

		# Add modules dependency to executables
		if(${ERM_MODULE}_EXECUTABLE)
			list(APPEND ${ERM_MODULE}_LINK_PRIVATE ERM_Modules_static)
			set(${ERM_MODULE}_LINK_PRIVATE "${${ERM_MODULE}_LINK_PRIVATE}" CACHE STRING "" FORCE)
		endif()
		
		# Add refl-cpp dependency to all modules
		list(APPEND ${ERM_MODULE}_LINK_PUBLIC refl-cpp)
		set(${ERM_MODULE}_LINK_PUBLIC "${${ERM_MODULE}_LINK_PUBLIC}" CACHE STRING "" FORCE)
	endforeach()
endfunction()

function(modules_pre_generate_callback)
	set(AllModulesIncludes "")
	set(AllModulesList "")

	foreach(ERM_MODULE ${ERM_MODULES})
		if("${ERM_MODULE}" STREQUAL "ERM_Modules")
			continue()
		endif()

		string(APPEND AllModulesIncludes "#include <erm/${ERM_MODULE}.h>\n")
		string(APPEND AllModulesList "\n\t${ERM_MODULE},")

		set(ModuleDependenciesForwardDeclarations "")
		set(ModuleDependenciesList "")
		set(ModuleName "${ERM_MODULE}")

		foreach(DEP ${${ERM_MODULE}_ALL_DEPS})
			string(REPLACE "_static" "" DEP ${DEP})
			string(REPLACE "_shared" "" DEP ${DEP})
			string(REPLACE "_int" "" DEP ${DEP})
			string(REPLACE "_obj" "" DEP ${DEP})

			if("${DEP}" STREQUAL "ERM_Modules")
				continue()
			endif()

			list(FIND ERM_MODULES ${DEP} INDEX)

			if(${INDEX} GREATER_EQUAL 0)
				string(APPEND ModuleDependenciesForwardDeclarations "class ${DEP};\n")
				string(APPEND ModuleDependenciesList "${DEP}, ")
			endif()
		endforeach()
		
		if(ModuleDependenciesList)
			erm_string_substr_end("${ModuleDependenciesList}" 2 ModuleDependenciesList)
		endif()

		# Configure ERM_Module.h
		set(MODULE_HEADER_PATH "${${ERM_MODULE}_GENERATED_DIR}/common/erm/${ERM_MODULE}.h")
		configure_file(
			"${ERM_Modules_MODULE_DIR}/Templates/ERM_Module.h"
			"${MODULE_HEADER_PATH}"
			@ONLY
		)
		list(APPEND ${ERM_MODULE}_GENERATED_CPP_HEADERS "${MODULE_HEADER_PATH}")
		set(${ERM_MODULE}_GENERATED_CPP_HEADERS "${${ERM_MODULE}_GENERATED_CPP_HEADERS}" CACHE STRING "" FORCE)

		list(APPEND ERM_Modules_PUBLIC_INCLUDE_DIRS "${${ERM_MODULE}_GENERATED_DIR}/common")
		set(ERM_Modules_PUBLIC_INCLUDE_DIRS "${ERM_Modules_PUBLIC_INCLUDE_DIRS}" CACHE STRING "" FORCE)

		list(APPEND ERM_Modules_GENERATED_CPP_HEADERS "${MODULE_HEADER_PATH}")
		set(ERM_Modules_GENERATED_CPP_HEADERS "${ERM_Modules_GENERATED_CPP_HEADERS}" CACHE STRING "" FORCE)

		if(${ERM_MODULE}_EXECUTABLE)
			# Configure dummy_main.cpp
			set(MODULE_MAIN_PATH "${${ERM_MODULE}_GENERATED_DIR}/common/erm/${ERM_MODULE}_main.cpp")
			configure_file(
				"${ERM_Modules_MODULE_DIR}/Templates/module_main.cpp"
				"${MODULE_MAIN_PATH}"
				@ONLY
			)
			set(${ERM_MODULE}_MODULE_MAIN_PATH "${MODULE_MAIN_PATH}" CACHE PATH "" FORCE)
		endif()
	endforeach()

	# Configure AllHeaders.h
	if(AllModulesList)
		erm_string_substr_end("${AllModulesList}" 1 AllModulesList)
	endif()

	if(AllModulesIncludes)
		erm_string_substr_end("${AllModulesIncludes}" 1 AllModulesIncludes)
	endif()

	set(ALL_MODULES_PATH "${ERM_Modules_GENERATED_DIR}/common/erm/AllModules.h")
	configure_file(
			"${ERM_Modules_MODULE_DIR}/Templates/AllModules.h"
			"${ALL_MODULES_PATH}"
			@ONLY
	)
	list(APPEND ERM_Modules_GENERATED_CPP_HEADERS "${ALL_MODULES_PATH}")
	set(ERM_Modules_GENERATED_CPP_HEADERS "${ERM_Modules_GENERATED_CPP_HEADERS}" CACHE STRING "" FORCE)
endfunction()


erm_module_setup(
	VERSION 0.0.1
	DESCRIPTION "ERM Modules"
	POST_SETUP_CALLBACK modules_post_setup_callback
	PRE_GENERATE_CALLBACK modules_pre_generate_callback
	LANGUAGES CXX
	LINK_PUBLIC
		refl-cpp
)
