function(modules_pre_generate_callback)
	erm_get_module_property(ERM_MODULES_BASE_DIR ERM_Modules BASE_DIR)
	set(AllModulesIncludes "")
	set(AllModulesList "")

	foreach(ERM_MODULE ${ERM_MODULES})
		erm_get_module_property(MODULE_STANDALONE ${ERM_MODULE} STANDALONE)
		if(MODULE_STANDALONE)
			continue()
		endif()

		string(APPEND AllModulesIncludes "#include <erm/${ERM_MODULE}.h>\n")
		string(APPEND AllModulesList "\n\t${ERM_MODULE},")

		set(ModuleDependenciesForwardDeclarations "")
		set(ModuleDependenciesList "")
		set(ModuleName "${ERM_MODULE}")

		erm_get_module_property(MODULE_ALL_DEPS ${ERM_MODULE} ALL_DEPS)
		foreach(DEP ${MODULE_ALL_DEPS})
			string(REPLACE "_static" "" DEP ${DEP})
			string(REPLACE "_shared" "" DEP ${DEP})
			string(REPLACE "_int" "" DEP ${DEP})
			string(REPLACE "_obj" "" DEP ${DEP})

			if(NOT TARGET ${DEP})
				continue()
			endif()

			erm_get_module_property(DEP_STANDALONE ${DEP} STANDALONE)
			if(NOT DEFINED DEP_STANDALONE OR DEP_STANDALONE)
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
		erm_get_module_property(MODULE_GENERATED_DIR ${ERM_MODULE} GENERATED_DIR)
		set(MODULE_HEADER_PATH "${MODULE_GENERATED_DIR}/common/erm/${ERM_MODULE}.h")
		configure_file(
			"${ERM_MODULES_BASE_DIR}/Templates/ERM_Module.h"
			"${MODULE_HEADER_PATH}"
			@ONLY
		)
		
		erm_add_to_module_property(
			TARGET ${ERM_MODULE}
			PROPERTY GENERATED_CPP_HEADERS
			VALUES "${MODULE_HEADER_PATH}"
		)

		erm_add_to_module_property(
			TARGET ERM_Modules
			PROPERTY PUBLIC_INCLUDE_DIRS
			VALUES "${MODULE_GENERATED_DIR}/common"
		)

		erm_add_to_module_property(
			TARGET ERM_Modules
			PROPERTY GENERATED_CPP_HEADERS
			VALUES "${MODULE_HEADER_PATH}"
		)

		erm_get_module_property(MODULE_EXECUTABLE ${ERM_MODULE} EXECUTABLE)
		if(MODULE_EXECUTABLE)
			# Configure dummy_main.cpp
			set(MAIN_PATH "${MODULE_GENERATED_DIR}/common/erm/${ERM_MODULE}_main.cpp")
			configure_file(
				"${ERM_MODULES_BASE_DIR}/Templates/module_main.cpp"
				"${MAIN_PATH}"
				@ONLY
			)
			erm_add_to_module_property(
				TARGET ${ERM_MODULE}
				PROPERTY MODULE_MAIN_PATH
				VALUES "${MAIN_PATH}"
			)
		endif()
	endforeach()

	# Configure AllHeaders.h
	if(AllModulesList)
		erm_string_substr_end("${AllModulesList}" 1 AllModulesList)
	endif()

	if(AllModulesIncludes)
		erm_string_substr_end("${AllModulesIncludes}" 1 AllModulesIncludes)
	endif()

	erm_get_module_property(ERM_MODULES_GENERATED_DIR ERM_Modules GENERATED_DIR)
	set(ALL_MODULES_PATH "${ERM_MODULES_GENERATED_DIR}/common/erm/AllModules.h")
	configure_file(
			"${ERM_MODULES_BASE_DIR}/Templates/AllModules.h"
			"${ALL_MODULES_PATH}"
			@ONLY
	)
	erm_add_to_module_property(
		TARGET ERM_Modules
		PROPERTY GENERATED_CPP_HEADERS
		VALUES "${ALL_MODULES_PATH}"
	)
endfunction()
