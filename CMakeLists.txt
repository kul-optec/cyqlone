cmake_minimum_required(VERSION 3.24...4.1)
set(CMAKE_CXX_SCAN_FOR_MODULES Off)
project(cyqlone
    VERSION 0.0.1
    DESCRIPTION "Parallel solver for systems with optimal control structure."
    HOMEPAGE_URL "https://github.com/kul-optec/cyqlone"
    LANGUAGES CXX
)
include(CTest)
include(CMakeDependentOption)
set(PY_VERSION_SUFFIX ".dev0")

# Options
option(CYQLONE_WITH_BLASFEO
    "Benchmarks comparing to BLASFEO Riccati recursion" Off)
option(CYQLONE_WITH_QPALM
    "Build the QPALM solver" On)
option(CYQLONE_COMPENSATE_SUM_LINE_SEARCH
    "Use the Kahan–Babuška–Neumaier algorithm in the line search" Off)
option(CYQLONE_QUADRUPLE_SUM_LINE_SEARCH
    "Use quadruple precision in the line search" Off)
cmake_dependent_option(CYQLONE_WITH_EXAMPLE_PROBLEMS
    "Build the simple OCP example problems" ${BUILD_TESTING}
    CYQLONE_WITH_QPALM Off)
# Enable/disable optional components
option(CYQLONE_WITH_TESTS
    "Build the tests" ${BUILD_TESTING})
option(CYQLONE_WITH_BENCHMARKS
    "Build the benchmarks" Off)
option(CYQLONE_WITH_PYTHON
    "Build the Python interface" Off)
option(CYQLONE_WITH_PYTHON_DISPATCH
    "Build the AVX2/AVX-512 dispatch logic for the Python interface" Off)
option(CYQLONE_WITH_MATIO
    "Use the matio library to save OCPs to .mat files" Off)
option(CYQLONE_WITH_SKA_SORT
    "Use the radix sort implementation provided by skarupke/ska_sort" Off)
# Developer options
option(CYQLONE_WARNINGS_AS_ERRORS
    "Enable -Werror or /WX" Off)
option(CYQLONE_WITH_COVERAGE
    "Generate coverage information" Off)
set(CYQLONE_DOXYFILE "Doxyfile" CACHE FILEPATH
    "The Doxyfile to use for the docs target")
option(CYQLONE_WITH_ACCURATE_BUILD_TIME
    "Update the build time on every build" On)
option(CYQLONE_DEVELOPER_MODE
    "Enable developer options such as sccache and colored compiler output" Off)

# Installation paths
include(GNUInstallDirs)
set(CYQLONE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}"
    CACHE PATH "Installation directory for archives and libraries")
set(CYQLONE_INSTALL_CMAKEDIR "${CYQLONE_INSTALL_LIBDIR}/cmake/cyqlone"
    CACHE PATH "Installation directory for CMake configuration files")
set(CYQLONE_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}"
    CACHE PATH "Installation directory for binaries and DLLs")
set(CYQLONE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}"
    CACHE PATH "Installation directory for headers")
set(CYQLONE_INSTALL_PYSTUBSDIR ""
    CACHE PATH "Installation directory for Python stubs (relative to Python package)")
option(CYQLONE_STANDALONE
    "Install with relative RPATH to locate its own shared libraries" On)

# Development
if (PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS On)
endif()
if (CYQLONE_DEVELOPER_MODE)
    include(cmake/Develop.cmake)
endif()

# Compiler warnings
include(cmake/Warnings.cmake)
add_warnings_target(warnings ${CYQLONE_WARNINGS_AS_ERRORS})
add_library(cyqlone::warnings ALIAS warnings)

# Compiler options
if (MSVC)
    add_compile_options("/bigobj")
endif()
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_RELWITHDEBINFO_POSTFIX "_rd")
set(CMAKE_MINSIZEREL_POSTFIX "_rs")

# Coverage
if (CYQLONE_WITH_COVERAGE)
    add_custom_target(coverage
        ${CMAKE_CURRENT_LIST_DIR}/scripts/coverage.sh
        ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL)
    add_compile_options("--coverage")
    add_link_options("--coverage")
    add_dependencies(coverage cyqlone::tests)
endif()

# Documentation
find_package(Doxygen)
if (DOXYGEN_FOUND)
    add_custom_target(docs Doxygen::doxygen ${CYQLONE_DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/doxygen)
endif()

# Libraries
add_subdirectory(src)

# Tests
if (CYQLONE_WITH_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Benchmarks
if (CYQLONE_WITH_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

add_subdirectory(interfaces)
