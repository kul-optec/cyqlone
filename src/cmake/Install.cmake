include(${PROJECT_SOURCE_DIR}/cmake/Debug.cmake)

# Set the runtime linker/loader search paths to make cyqlone stand-alone
cmake_path(RELATIVE_PATH CYQLONE_INSTALL_LIBDIR
           BASE_DIRECTORY CYQLONE_INSTALL_BINDIR
           OUTPUT_VARIABLE CYQLONE_INSTALL_LIBRELBINDIR)

function(cyqlone_add_if_target_exists OUT)
    foreach(TGT IN LISTS ARGN)
        if (TARGET ${TGT})
            list(APPEND ${OUT} ${TGT})
        endif()
    endforeach()
    set(${OUT} ${${OUT}} PARENT_SCOPE)
endfunction()

include(CMakePackageConfigHelpers)

set(CYQLONE_INSTALLED_COMPONENTS)
macro(cyqlone_install_config PKG COMP)
    # Install the target CMake definitions
    install(EXPORT cyqlone${PKG}Targets
        FILE cyqlone${PKG}Targets.cmake
        DESTINATION "${CYQLONE_INSTALL_CMAKEDIR}"
            COMPONENT ${COMP}
        NAMESPACE cyqlone::)
    # Add all targets to the build tree export set
    export(EXPORT cyqlone${PKG}Targets
        FILE "${PROJECT_BINARY_DIR}/cyqlone${PKG}Targets.cmake"
        NAMESPACE cyqlone::)
    # Generate the config file that includes the exports
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PKG}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/cyqlone${PKG}Config.cmake"
        INSTALL_DESTINATION "${CYQLONE_INSTALL_CMAKEDIR}"
        NO_SET_AND_CHECK_MACRO)
    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/cyqlone${PKG}ConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMinorVersion)
    # Install the cyqloneConfig.cmake and cyqloneConfigVersion.cmake
    install(FILES
        "${PROJECT_BINARY_DIR}/cyqlone${PKG}Config.cmake"
        "${PROJECT_BINARY_DIR}/cyqlone${PKG}ConfigVersion.cmake"
        DESTINATION "${CYQLONE_INSTALL_CMAKEDIR}"
            COMPONENT ${COMP})
    list(APPEND CYQLONE_OPTIONAL_COMPONENTS ${PKG})
endmacro()

macro(cyqlone_install_cmake FILES COMP)
    # Install a CMake script
    install(FILES ${FILES}
        DESTINATION "${CYQLONE_INSTALL_CMAKEDIR}"
            COMPONENT ${COMP})
endmacro()

set(CYQLONE_INSTALLED_TARGETS_MSG "\nSummary of cyqlone components and targets to install:\n\n")

# Install the cyqlone core libraries
set(CYQLONE_CORE_HIDDEN_TARGETS warnings common_options)
set(CYQLONE_CORE_TARGETS cyqlone)
if (CYQLONE_CORE_TARGETS)
    install(TARGETS ${CYQLONE_CORE_HIDDEN_TARGETS} ${CYQLONE_CORE_TARGETS}
        EXPORT cyqloneCoreTargets
        RUNTIME DESTINATION "${CYQLONE_INSTALL_BINDIR}"
            COMPONENT lib
        LIBRARY DESTINATION "${CYQLONE_INSTALL_LIBDIR}"
            COMPONENT lib
            NAMELINK_COMPONENT dev
        ARCHIVE DESTINATION "${CYQLONE_INSTALL_LIBDIR}"
            COMPONENT dev
        FILE_SET HEADERS DESTINATION "${CYQLONE_INSTALL_INCLUDEDIR}"
            COMPONENT dev)
    cyqlone_install_config(Core dev)
    list(JOIN CYQLONE_CORE_TARGETS ", " TGTS)
    string(APPEND CYQLONE_INSTALLED_TARGETS_MSG " * Core:   ${TGTS}\n")
    list(APPEND CYQLONE_INSTALL_TARGETS ${CYQLONE_CORE_TARGETS})
endif()

# Install the QPALM targets
set(CYQLONE_QPALM_TARGETS)
cyqlone_add_if_target_exists(CYQLONE_QPALM_TARGETS "qpalm" "ladel-backend")
if (CYQLONE_QPALM_TARGETS)
    install(TARGETS warnings common_options ${CYQLONE_QPALM_TARGETS}
        EXPORT cyqloneQPALMTargets
        RUNTIME DESTINATION "${CYQLONE_INSTALL_BINDIR}"
            COMPONENT lib
        LIBRARY DESTINATION "${CYQLONE_INSTALL_LIBDIR}"
            COMPONENT lib
            NAMELINK_COMPONENT dev
        ARCHIVE DESTINATION "${CYQLONE_INSTALL_LIBDIR}"
            COMPONENT dev
        FILE_SET HEADERS DESTINATION "${CYQLONE_INSTALL_INCLUDEDIR}"
            COMPONENT dev)
    cyqlone_install_config(QPALM dev)
    list(JOIN CYQLONE_QPALM_TARGETS ", " TGTS)
    string(APPEND CYQLONE_INSTALLED_TARGETS_MSG " * QPALM:  ${TGTS}\n")
    list(APPEND CYQLONE_INSTALL_TARGETS ${CYQLONE_QPALM_TARGETS})
endif()

# Install the extra targets
set(CYQLONE_EXTRA_TARGETS)
cyqlone_add_if_target_exists(CYQLONE_EXTRA_TARGETS "example-problems")
if (CYQLONE_EXTRA_TARGETS)
    install(TARGETS warnings common_options ${CYQLONE_EXTRA_TARGETS}
        EXPORT cyqloneExtraTargets
        RUNTIME DESTINATION "${CYQLONE_INSTALL_BINDIR}"
            COMPONENT lib
        LIBRARY DESTINATION "${CYQLONE_INSTALL_LIBDIR}"
            COMPONENT lib
            NAMELINK_COMPONENT dev
        ARCHIVE DESTINATION "${CYQLONE_INSTALL_LIBDIR}"
            COMPONENT dev
        FILE_SET HEADERS DESTINATION "${CYQLONE_INSTALL_INCLUDEDIR}"
            COMPONENT dev)
    cyqlone_install_config(Extra dev)
    list(JOIN CYQLONE_EXTRA_TARGETS ", " TGTS)
    string(APPEND CYQLONE_INSTALLED_TARGETS_MSG " * Extra:  ${TGTS}\n")
    list(APPEND CYQLONE_INSTALL_TARGETS ${CYQLONE_EXTRA_TARGETS})
endif()

# Install the debug files
foreach(target IN LISTS CYQLONE_CORE_TARGETS CYQLONE_EXTRA_TARGETS)
    get_target_property(target_type ${target} TYPE)
    if (${target_type} STREQUAL "SHARED_LIBRARY")
        cyqlone_install_debug_syms(${target} debug
                                  ${CYQLONE_INSTALL_LIBDIR}
                                  ${CYQLONE_INSTALL_BINDIR})
    elseif (${target_type} STREQUAL "EXECUTABLE")
        cyqlone_install_debug_syms(${target} debug
                                  ${CYQLONE_INSTALL_BINDIR}
                                  ${CYQLONE_INSTALL_BINDIR})
    endif()
endforeach()

# Make stand-alone
if (CYQLONE_STANDALONE)
    foreach(target IN LISTS CYQLONE_CORE_TARGETS)
        set_target_properties(${TGT} PROPERTIES
            INSTALL_RPATH "$ORIGIN;$ORIGIN/${CYQLONE_INSTALL_LIBRELBINDIR}")
    endforeach()
endif()

# Generate the main config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${PROJECT_BINARY_DIR}/cyqloneConfig.cmake"
    INSTALL_DESTINATION "${CYQLONE_INSTALL_CMAKEDIR}"
    NO_SET_AND_CHECK_MACRO)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/cyqloneConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMinorVersion)
# Install the main cyqloneConfig.cmake and cyqloneConfigVersion.cmake files
install(FILES
    "${PROJECT_BINARY_DIR}/cyqloneConfig.cmake"
    "${PROJECT_BINARY_DIR}/cyqloneConfigVersion.cmake"
    DESTINATION "${CYQLONE_INSTALL_CMAKEDIR}"
        COMPONENT dev)

# Print the components and targets we're going to install
message(${CYQLONE_INSTALLED_TARGETS_MSG})
