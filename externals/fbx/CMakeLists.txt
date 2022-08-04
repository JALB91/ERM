project(fbx_sdk)

if(MSVC OR WIN)
    set(FBX_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/debug/libfbxsdk.lib")
    set(FBX_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/release/libfbxsdk.lib")
elseif(XCODE OR APPLE)
    set(FBX_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/clang/debug/libfbxsdk.a")
    set(FBX_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/clang/release/libfbxsdk.a")
else()
    message(FATAL_ERROR "Platform not supported")
endif()

if("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xMSVC")
    if (NOT DEFINED FBX_MSVC_RT_DLL OR FBX_MSVC_RT_DLL)
        set(FBX_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/debug/libfbxsdk-md.lib")
        set(XML_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/debug/libxml2-md.lib")
        set(Z_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/debug/zlib-md.lib")

        set(FBX_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/release/libfbxsdk-md.lib")
        set(XML_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/release/libxml2-md.lib")
        set(Z_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/release/zlib-md.lib")
    else()
        set(FBX_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/debug/libfbxsdk-mt.lib")
        set(XML_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/debug/libxml2-mt.lib")
        set(Z_LIB_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/debug/zlib-mt.lib")

        set(FBX_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/release/libfbxsdk-mt.lib")
        set(XML_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/release/libxml2-mt.lib")
        set(Z_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/win64/release/zlib-mt.lib")
    endif()
else()
    set(XML_SEARCH_LIB_NAMES "xml2")
    set(Z_SEARCH_LIB_NAMES "z")

    find_library(XML_LIB ${XML_SEARCH_LIB_NAMES})
    find_library(Z_LIB ${Z_SEARCH_LIB_NAMES})

    # Searches debug version also
    find_library(XML_LIB_DEBUG ${XML_SEARCH_LIB_NAMES})
    find_library(Z_LIB_DEBUG ${Z_SEARCH_LIB_NAMES})
endif()

# for whatever reason on apple it will need iconv as well?!
if(APPLE)
    find_library(ICONV_LIB iconv)

    # no special debug search here as mac only anyway
    if(NOT ICONV_LIB)
        message(FATAL_ERROR "FBX found but required iconv was not found!")
    endif()
endif()

if(NOT XML_LIB)
    message(FATAL_ERROR "FBX found but required libxml2 was not found!")
endif()
if(NOT Z_LIB)
    message(FATAL_ERROR "FBX found but required zlib was not found!")
endif()

add_library("${PROJECT_NAME}" STATIC IMPORTED GLOBAL)

target_link_libraries(
    "${PROJECT_NAME}"
    INTERFACE
        $<$<CONFIG:Release>:${XML_LIB}>
        $<$<CONFIG:Release>:${Z_LIB}>
        $<$<CONFIG:Debug>:${XML_LIB_DEBUG}>
        $<$<CONFIG:Debug>:${Z_LIB_DEBUG}>
        "${ICONV_LIB}"
)

set_target_properties(
    "${PROJECT_NAME}"
    PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${FBX_LIB_DEBUG}"
        IMPORTED_IMPLIB_RELEASE "${FBX_LIB}"
        IMPORTED_LOCATION_DEBUG "${FBX_LIB_DEBUG}"
        IMPORTED_LOCATION_RELEASE "${FBX_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
