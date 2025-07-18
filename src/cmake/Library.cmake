include(GenerateExportHeader)

# Sets the default visibility to "hidden", and prevents exporting symbols from
# static libraries into shared libraries on Linux.
function(cyqlone_configure_visibility target)
    set_target_properties(${target} PROPERTIES CXX_VISIBILITY_PRESET "hidden"
                                               C_VISIBILITY_PRESET "hidden"
                                               VISIBILITY_INLINES_HIDDEN true)
    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        target_link_options(${target} PRIVATE
            $<$<LINK_LANGUAGE:C,CXX>:LINKER:--exclude-libs,ALL>)
    endif()
endfunction()

add_library(common_options INTERFACE)
if (MSVC)
    target_compile_options(common_options INTERFACE "/utf-8")
endif()
add_library(cyqlone::common_options ALIAS common_options)

# Configure the given library target with some sensible default options:
# Takes care of configuring DLL export headers, visibility flags, warning flags,
# SO version, C++ standard version, warning flags, and creation of an alias.
function(cyqlone_configure_library tgt)
    cmake_parse_arguments(CYQLONE_CFG_LIB "" "EXPORT_PREFIX" "" ${ARGN})
    cmake_path(SET export_path "")
    set(name_prefix "")
    if (DEFINED CYQLONE_CFG_LIB_EXPORT_PREFIX)
        cmake_path(APPEND export_path ${CYQLONE_CFG_LIB_EXPORT_PREFIX})
        set(name_prefix "${CYQLONE_CFG_LIB_EXPORT_PREFIX}-")
    endif()
    set_property(TARGET ${tgt} PROPERTY OUTPUT_NAME "${name_prefix}${tgt}")
    generate_export_header(${tgt} BASE_NAME "${name_prefix}${tgt}"
        EXPORT_FILE_NAME export/${export_path}/${tgt}/export.h)
    target_sources(${tgt} PUBLIC FILE_SET HEADERS
        BASE_DIRS   ${CMAKE_CURRENT_BINARY_DIR}/export
        FILES       ${CMAKE_CURRENT_BINARY_DIR}/export/${export_path}/${tgt}/export.h)
    set_target_properties(${tgt} PROPERTIES SOVERSION ${PROJECT_VERSION})
    cyqlone_configure_visibility(${tgt})
    target_compile_features(${tgt} PUBLIC cxx_std_20)
    if (CYQLONE_WITH_CXX_23)
        # Use C++23 to build everything, but don't require C++23 for installed
        # versions of the library (see cmake/CoreConfig.cmake.in)
        target_compile_features(${tgt} PUBLIC $<BUILD_INTERFACE:cxx_std_23>)
    endif()
    target_link_libraries(${tgt} PRIVATE cyqlone::warnings
                                 PUBLIC cyqlone::common_options)
    add_library(cyqlone::${tgt} ALIAS ${tgt})
endfunction()

# Configure the given interface library target with some sensible default
# options: C++ standard version, platform-specific flags
function(cyqlone_configure_interface_library tgt)
    target_compile_features(${tgt} INTERFACE cxx_std_20)
    if (CYQLONE_WITH_CXX_23)
        # Use C++23 to build everything, but don't require C++23 for installed
        # versions of the library (see cmake/CoreConfig.cmake.in)
        target_compile_features(${tgt} INTERFACE $<BUILD_INTERFACE:cxx_std_23>)
    endif()
    target_link_libraries(${tgt} INTERFACE cyqlone::common_options)
    add_library(cyqlone::${tgt} ALIAS ${tgt})
endfunction()
