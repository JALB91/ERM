if(MSVC OR WIN)
    set(FMOD_CORE_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/api/core/lib/Win/x64/fmod_vc.lib")
    set(FMOD_CORE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/api/core/lib/Win/x64/fmod.dll")
    set(FMOD_STUDIO_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/lib/Win/x64/fmodstudio_vc.lib")
    set(FMOD_STUDIO_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/lib/Win/x64/fmodstudio.dll")
elseif(XCODE OR APPLE)
    set(FMOD_CORE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/api/core/lib/OSX/libfmod.dylib")
    set(FMOD_STUDIO_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/lib/OSX/libfmodstudio.dylib")
else()
    message(FATAL_ERROR "Unsupported Platform")
endif()

# FMOD_CORE
project(fmod_core)

add_library("${PROJECT_NAME}" SHARED IMPORTED GLOBAL)

set_target_properties(
    "${PROJECT_NAME}"
    PROPERTIES
        IMPORTED_IMPLIB "${FMOD_CORE_IMPLIB}"
        IMPORTED_LOCATION "${FMOD_CORE_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/api/core/inc"
)

# FMOD_STUDIO
project(fmod_studio)

add_library("${PROJECT_NAME}" SHARED IMPORTED GLOBAL)

set_target_properties(
    "${PROJECT_NAME}"
    PROPERTIES
        IMPORTED_IMPLIB "${FMOD_STUDIO_IMPLIB}"
        IMPORTED_LOCATION "${FMOD_STUDIO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/inc"
)

if(MSVC OR WIN)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${FMOD_CORE_LIBRARY}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${FMOD_STUDIO_LIBRARY}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
endif(MSVC OR WIN)