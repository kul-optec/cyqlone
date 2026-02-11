cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

if (DEFINED ENV{MKLROOT})
    list(PREPEND CMAKE_FIND_ROOT_PATH "$ENV{MKLROOT}")
endif()

find_package(cyqlone CONFIG REQUIRED OPTIONAL_COMPONENTS QPALM Extra)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE cyqlone::cyqlone)
if (TARGET cyqlone::qpalm AND TARGET cyqlone::example-problems)
    target_compile_definitions(example PRIVATE WITH_QPALM)
    target_link_libraries(example PRIVATE cyqlone::qpalm cyqlone::example-problems)
endif()
