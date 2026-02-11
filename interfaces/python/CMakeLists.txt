set(CYQLONE_PYTHON_POSTFIX "_generic" CACHE STRING
    "Additional postfix in Python module file name")
set(PY_FULL_VERSION ${PROJECT_VERSION}${PY_VERSION_SUFFIX})
set(PY_BUILD_CMAKE_IMPORT_NAME "cyqlone" CACHE STRING "Python module name")

# Make sure that the Python and CMake versions match
if (DEFINED PY_BUILD_CMAKE_PACKAGE_VERSION)
    if (NOT "${PY_BUILD_CMAKE_PACKAGE_VERSION}" MATCHES "^${PY_FULL_VERSION}$")
        message(FATAL_ERROR "Version number does not match "
                             "(${PY_BUILD_CMAKE_PACKAGE_VERSION} - ${PY_FULL_VERSION}).")
    endif()
endif()

# Find Python and nanobind
if (CMAKE_CROSSCOMPILING AND NOT CMAKE_CROSSCOMPILING_EMULATOR)
    find_package(Python REQUIRED COMPONENTS Development.Module
                        OPTIONAL_COMPONENTS Development.SABIModule)
else()
    find_package(Python REQUIRED COMPONENTS Interpreter Development.Module
                        OPTIONAL_COMPONENTS Development.SABIModule)
endif()
find_package(nanobind REQUIRED CONFIG)
find_package(Eigen3 REQUIRED)

# Determine whether we're doing a stable or a free-threaded build
set(USE_SABI)
set(NANOBIND_TARGET "nanobind-cyqlone")
if (PY_BUILD_CMAKE_PACKAGE_ABI_TAG MATCHES "(^|;)cp([0-9dmu]+)t($|;)")
    message(STATUS "Free-threaded build")
    string(APPEND NANOBIND_TARGET "-ft")
elseif (DEFINED PY_BUILD_CMAKE_PACKAGE_USE_SABI)
    message(STATUS "Stable ABI build (${PY_BUILD_CMAKE_PACKAGE_USE_SABI})")
    string(APPEND NANOBIND_TARGET "-abi3")
    set(USE_SABI USE_SABI ${PY_BUILD_CMAKE_PACKAGE_USE_SABI})
    set(USE_LIMITED_API ${PY_BUILD_CMAKE_PACKAGE_LIMITED_API})
endif()

# Build the nanobind library
nanobind_build_library(${NANOBIND_TARGET} AS_SYSINCLUDE)
if (USE_LIMITED_API)
    target_compile_definitions(${NANOBIND_TARGET} PRIVATE
        Py_LIMITED_API=${USE_LIMITED_API})
endif()
install(TARGETS ${NANOBIND_TARGET}
        EXCLUDE_FROM_ALL
        COMPONENT python_nanobind
        DESTINATION ${PY_BUILD_CMAKE_IMPORT_NAME})

# Configure the dynamic loading paths to locate the nanobind libraries.
function(cyqlone_rpath_origin tgt)
    if(APPLE)
        set_target_properties(${tgt} PROPERTIES INSTALL_RPATH "@loader_path")
    else()
        set_target_properties(${tgt} PROPERTIES INSTALL_RPATH "$ORIGIN")
    endif()
endfunction()

# Configure common options for Python extension modules
function(cyqlone_configure_python_ext tgt)
    target_link_libraries(${tgt} PRIVATE ${NANOBIND_TARGET})
    cyqlone_rpath_origin(${tgt})
    nanobind_compile_options(${tgt})
    nanobind_link_options(${tgt})
    cyqlone_configure_visibility(${tgt})
    target_compile_definitions(${tgt} PRIVATE
        MODULE_NAME=$<TARGET_FILE_BASE_NAME:${tgt}>
        VERSION_INFO="${PY_FULL_VERSION}")
endfunction()

# Build the architecture-specific Python extension
Python_add_library(_cyqlone MODULE WITH_SOABI ${USE_SABI} "src/cyqlone.py.cpp")
cyqlone_configure_python_ext(_cyqlone)
target_link_libraries(_cyqlone PRIVATE Eigen3::Eigen cyqlone::qpalm cyqlone::example-problems)
target_link_libraries(_cyqlone PRIVATE cyqlone::warnings)
set_target_properties(_cyqlone PROPERTIES
    RELEASE_POSTFIX "${CYQLONE_PYTHON_POSTFIX}"
    DEBUG_POSTFIX "${CYQLONE_PYTHON_POSTFIX}"
    RELWITHDEBINFO_POSTFIX "${CYQLONE_PYTHON_POSTFIX}"
    MINSIZEREL_POSTFIX "${CYQLONE_PYTHON_POSTFIX}")

# Install the Python module
install(TARGETS _cyqlone
        EXCLUDE_FROM_ALL
        COMPONENT python_modules
        DESTINATION ${PY_BUILD_CMAKE_IMPORT_NAME})

# Build the common library
if (NOT CYQLONE_WITH_PYTHON_ARCH_SPECIFIC_ONLY)
    Python_add_library(_common MODULE WITH_SOABI ${USE_SABI} "src/common.py.cpp")
    cyqlone_configure_python_ext(_common)
    target_link_libraries(_common PRIVATE cyqlone::qpalm cyqlone::example-problems)
    target_link_libraries(_common PRIVATE cyqlone::warnings)
    cyqlone_configure_visibility(_common)
    set_target_properties(_common PROPERTIES
        RELEASE_POSTFIX "" DEBUG_POSTFIX "" RELWITHDEBINFO_POSTFIX "" MINSIZEREL_POSTFIX "")
    if (CYQLONE_WITH_PYTHON_DISPATCH)
        find_package(CpuFeatures REQUIRED)
        target_link_libraries(_common PRIVATE CpuFeatures::cpu_features)
        target_compile_definitions(_common PRIVATE CYQLONE_WITH_PYTHON_DISPATCH)
    endif()
    install(TARGETS _common
        EXCLUDE_FROM_ALL
        COMPONENT python_modules
        DESTINATION ${PY_BUILD_CMAKE_IMPORT_NAME})
    install(IMPORTED_RUNTIME_ARTIFACTS _common RUNTIME_DEPENDENCY_SET cyqlone_common_runtime_deps)
    install(RUNTIME_DEPENDENCY_SET cyqlone_common_runtime_deps
        COMPONENT python_modules
        PRE_INCLUDE_REGEXES ".*guanaqo.*"
        PRE_EXCLUDE_REGEXES ".*"
        LIBRARY DESTINATION ${PY_BUILD_CMAKE_IMPORT_NAME})
endif()

# Generate stubs for the Python module
set(CYQLONE_WITH_PY_STUBS_DEFAULT On)
if (CMAKE_CROSSCOMPILING)
    set(CYQLONE_WITH_PY_STUBS_DEFAULT Off)
endif()
option(CYQLONE_WITH_PY_STUBS
    "Generate Python stub files (.pyi) for the Python module."
    ${CYQLONE_WITH_PY_STUBS_DEFAULT})
if (CYQLONE_WITH_PY_STUBS)
    install(CODE "set(ENV{CYQLONE_VARIANT} \"${CYQLONE_PYTHON_POSTFIX}\")" COMPONENT python_stubs)
    nanobind_add_stub(
        cyqlone_stubs
        INSTALL_TIME
        COMPONENT python_stubs
        MODULE "${PY_BUILD_CMAKE_IMPORT_NAME}.$<TARGET_FILE_BASE_NAME:_cyqlone>"
        OUTPUT_PATH "${PY_BUILD_CMAKE_IMPORT_NAME}"
        RECURSIVE
        EXCLUDE_FROM_ALL
        VERBOSE)
    foreach(mod "__init__" "scalar" "simd4" "simd8")
        file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/stubs/${mod}.pyi"
             INPUT "${CMAKE_CURRENT_SOURCE_DIR}/stubs/${mod}.pyi")
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/stubs/${mod}.pyi"
                COMPONENT python_stubs
                DESTINATION "${PY_BUILD_CMAKE_IMPORT_NAME}/cyqlone")
    endforeach()
    if (NOT CYQLONE_WITH_PYTHON_ARCH_SPECIFIC_ONLY)
        nanobind_add_stub(
            cyqlone_stubs
            INSTALL_TIME
            COMPONENT python_stubs
            MODULE "${PY_BUILD_CMAKE_IMPORT_NAME}.$<TARGET_FILE_BASE_NAME:_common>"
            OUTPUT_PATH "${PY_BUILD_CMAKE_IMPORT_NAME}"
            RECURSIVE
            EXCLUDE_FROM_ALL
            VERBOSE)
    endif()
endif()
