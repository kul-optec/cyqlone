cmake_minimum_required(VERSION 3.24...4.1)
project(cyqlone-examples)

if (NOT TARGET cyqlone::cyqlone)
    find_package(cyqlone REQUIRED)
endif()

add_executable(solve-block-tridiagonal solve-block-tridiagonal.cpp)
target_link_libraries(solve-block-tridiagonal PRIVATE cyqlone::cyqlone)
if (NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(solve-block-tridiagonal PRIVATE cyqlone::warnings)
endif()

add_executable(solve-ocp solve-ocp.cpp)
target_link_libraries(solve-ocp PRIVATE cyqlone::cyqlone)
if (NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(solve-ocp PRIVATE cyqlone::warnings)
endif()
