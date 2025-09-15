set(PY_FULL_VERSION ${PROJECT_VERSION}${PY_VERSION_SUFFIX})
set(PY_BUILD_CMAKE_MODULE_NAME "cyqlone" CACHE STRING "Python module name")
set(PY_BUILD_CMAKE_PACKAGE_NAME "cyqlone" CACHE STRING "Python package name")

# Make sure that the Python and CMake versions match
if (DEFINED PY_BUILD_CMAKE_PACKAGE_VERSION)
    if (NOT "${PY_BUILD_CMAKE_PACKAGE_VERSION}" MATCHES "^${PY_FULL_VERSION}$")
        message(FATAL_ERROR "Version number does not match "
                             "(${PY_BUILD_CMAKE_PACKAGE_VERSION} - ${PY_FULL_VERSION}).")
    endif()
endif()

if (CMAKE_CROSSCOMPILING AND NOT (APPLE AND "$ENV{CIBUILDWHEEL}" STREQUAL "1"))
    find_package(Python3 REQUIRED COMPONENTS Development.Module)
else()
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
endif()
find_package(pybind11 ${ARGN} REQUIRED CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
find_package(Eigen3 REQUIRED)

# Compile the example Python module
pybind11_add_module(_cyqlone MODULE
    "src/cyqlone.py.cpp"
)
target_include_directories(_cyqlone PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(_cyqlone PRIVATE
    pybind11::pybind11 Eigen3::Eigen cyqlone::qpalm cyqlone::example-problems)
target_link_libraries(_cyqlone PRIVATE cyqlone::warnings)
target_compile_definitions(_cyqlone PRIVATE
    MODULE_NAME=$<TARGET_FILE_BASE_NAME:_cyqlone>)
set_target_properties(_cyqlone PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN true
    RELEASE_POSTFIX ""
    DEBUG_POSTFIX ""
    RELWITHDEBINFO_POSTFIX ""
    MINSIZEREL_POSTFIX ""
)
include(GenerateExportHeader)
set(CYQLONE_PYTHON_DEBUG_CONFIG "Debug" CACHE STRING "")
set_property(CACHE CYQLONE_PYTHON_DEBUG_CONFIG PROPERTY STRINGS
    ${CMAKE_CONFIGURATION_TYPES})
if (CYQLONE_PYTHON_DEBUG_CONFIG)
    string(TOUPPER ${CYQLONE_PYTHON_DEBUG_CONFIG} CYQLONE_PYTHON_DEBUG_CONFIG_UP)
    set_target_properties(_cyqlone PROPERTIES
        ${CYQLONE_PYTHON_DEBUG_CONFIG_UP}_POSTFIX "_d"
        PDB_NAME_${CYQLONE_PYTHON_DEBUG_CONFIG_UP} "_cyqlone_d")
endif()
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_options(_cyqlone PRIVATE "LINKER:--exclude-libs,ALL")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(_cyqlone PRIVATE "/bigobj")
endif()

if (DEFINED PY_BUILD_CMAKE_VERSION)
    set(CYQLONE_MODULE_INSTALL_DIR "")
else()
    set(CYQLONE_MODULE_INSTALL_DIR "${CYQLONE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages")
endif()

# Install the Python source code
install(DIRECTORY ${PROJECT_SOURCE_DIR}/python/${PY_BUILD_CMAKE_MODULE_NAME}
        EXCLUDE_FROM_ALL
        COMPONENT python_source
        DESTINATION "${CYQLONE_MODULE_INSTALL_DIR}"
        PATTERN "__pycache__" EXCLUDE)

# Install the Python module
install(TARGETS _cyqlone
        EXCLUDE_FROM_ALL
        COMPONENT python_modules
        DESTINATION ./${CYQLONE_MODULE_INSTALL_DIR}/${PY_BUILD_CMAKE_MODULE_NAME})

# Strip and install debug information
include(${PROJECT_SOURCE_DIR}/cmake/Debug.cmake)
cyqlone_install_debug_syms(_cyqlone python_modules_debug
    ./${CYQLONE_MODULE_INSTALL_DIR}/${PY_BUILD_CMAKE_MODULE_NAME}
    ./${CYQLONE_MODULE_INSTALL_DIR}/${PY_BUILD_CMAKE_MODULE_NAME})

# Generate stubs for the Python module
set(CYQLONE_WITH_PY_STUBS_DEFAULT On)
if (CMAKE_CROSSCOMPILING)
    set(CYQLONE_WITH_PY_STUBS_DEFAULT Off)
endif()
option(CYQLONE_WITH_PY_STUBS
    "Generate Python stub files (.pyi) for the Python module."
    ${CYQLONE_WITH_PY_STUBS_DEFAULT})
if (CYQLONE_WITH_PY_STUBS)
    include(cmake/Pybind11Stubgen.cmake)
    pybind11_stubgen(_cyqlone
        COMPONENT python_stubs
        PACKAGE ${PY_BUILD_CMAKE_MODULE_NAME}
        PACKAGE_ROOT "${CYQLONE_MODULE_INSTALL_DIR}")
endif()
