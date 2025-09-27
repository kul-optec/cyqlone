cmake_minimum_required(VERSION 3.21...4.1)
project(cyqlone-test CXX)
enable_testing()
include(GoogleTest)

# Dependencies
find_package(GTest REQUIRED)
find_package(guanaqo REQUIRED)
if (NOT TARGET cyqlone::cyqlone)
    find_package(cyqlone REQUIRED OPTIONAL_COMPONENTS QPALM Extra)
endif()

add_executable(tests
    "cyqlone/test-cyqlone.cpp"
)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(tests PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(tests PRIVATE
    cyqlone::cyqlone cyqlone::warnings
    GTest::gtest_main)
if (MSVC)
    target_compile_definitions(tests PRIVATE __PRETTY_FUNCTION__=__FUNCSIG__)
endif()
if (TARGET cyqlone::qpalm)
    target_sources(tests PRIVATE "test-partition.cpp")
endif()
if (TARGET cyqlone::qpalm AND TARGET cyqlone::example-problems)
    find_package(Eigen3 REQUIRED)
    target_sources(tests PRIVATE "qpalm/test-qpalm-ocp.cpp")
    target_link_libraries(tests PRIVATE cyqlone::qpalm cyqlone::example-problems Eigen3::Eigen)
    find_package(QPALM QUIET OPTIONAL_COMPONENTS cxx)
    if (TARGET QPALM::qpalm_cxx)
        target_sources(tests PRIVATE "qpalm/test-qpalm-ben.cpp")
        target_link_libraries(tests PRIVATE QPALM::qpalm_cxx)
    endif()
endif()
if (CYQLONE_WITH_MATIO)
    target_sources(tests PRIVATE "test-matio.cpp")
endif()

option(CYQLONE_FORCE_TEST_DISCOVERY
    "Query the test executable even when cross-compiling" Off)
if (NOT CMAKE_CROSSCOMPILING OR CYQLONE_FORCE_TEST_DISCOVERY)
    gtest_discover_tests(tests DISCOVERY_TIMEOUT 60)
endif()
add_executable(cyqlone::tests ALIAS tests)

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.21)
    # Copy DLLs for test executable
    add_custom_command(TARGET tests PRE_LINK COMMAND
        COMMAND ${CMAKE_COMMAND} -E
            $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:tests>>,copy,true>
            $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
        COMMAND_EXPAND_LISTS)
endif()
