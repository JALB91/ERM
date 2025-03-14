set(ALL_NN_SOURCES_HEADER "#if defined(ERM_NN_IMPL)
#error \"ERM_NN_IMPL already defined, this should not happen, did you include this file twice?\"
#endif

#define ERM_NN_IMPL

namespace erm::nn::internal {

static constexpr auto kERMNNCounterStart = __COUNTER__;

}

")

foreach(ERM_LIB_DIRECTORY ${ERM_LIBS_DIRECTORIES})
	erm_get_dir_name_from_path("${ERM_LIB_DIRECTORY}")
	if(${DIR_NAME}_NN_SOURCES)
		foreach(NN_SOURCE ${${DIR_NAME}_NN_SOURCES})
			cmake_path(GET NN_SOURCE EXTENSION LAST_ONLY FILE_EXTENSION)
			if(FILE_EXTENSION STREQUAL ".h" OR FILE_EXTENSION STREQUAL ".hpp")
				string(APPEND ALL_NN_SOURCES_HEADER "#include \"${NN_SOURCE}\"\n")
				list(APPEND ERM_NN_CPP_HEADERS ${NN_SOURCE})
			elseif(FILE_EXTENSION STREQUAL ".cpp")
				list(APPEND ERM_NN_CPP_SOURCES ${NN_SOURCE})
			endif()
		endforeach()
		list(APPEND ERM_NN_ALL_SOURCES ${${DIR_NAME}_NN_SOURCES})
        list(APPEND ERM_ALL_NN_DIRS "${ERM_LIB_DIRECTORY}/NN_Sources/common")
        list(APPEND ERM_ALL_NN_DIRS "${ERM_LIB_DIRECTORY}/NN_Sources/${TARGET_API}")
    endif()
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

file(MAKE_DIRECTORY "${ERM_GENERATED_DIR}/ERM_NN/erm/nn")
file(WRITE "${ERM_GENERATED_DIR}/ERM_NN/common/erm/nn/NNAllStatementHandlers.h" "${ALL_NN_SOURCES_HEADER}")
list(APPEND ERM_NN_NN_GEN_FILES "${ERM_GENERATED_DIR}/ERM_NN/common/erm/nn/NNAllStatementHandlers.h")
list(APPEND ERM_NN_ALL_SOURCES "${ERM_GENERATED_DIR}/ERM_NN/common/erm/nn/NNAllStatementHandlers.h")

erm_executable_project(0.0.1 "ERM NN lib")

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		${ERM_ALL_NN_DIRS}	
)

target_compile_definitions(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_ZONE_COLOR=0x000011
)

target_link_libraries(
	"${PROJECT_NAME}"
	PUBLIC 
        glm
		nlohmann_json
		magic_enum
		ERM_Utils
		ERM_Math
		ERM_FileSystem
		refl-cpp
)
