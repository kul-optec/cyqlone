include(cmake/Library.cmake)
find_package(guanaqo REQUIRED COMPONENTS BLAS)
find_package(batmat REQUIRED)

# Main library
add_library(cyqlone
    "cyqlone/src/cyqlone.cpp"
    "cyqlone/src/cyqlone-storage.cpp"
)
target_sources(cyqlone PUBLIC FILE_SET headers TYPE HEADERS
    BASE_DIRS "cyqlone/include"
    FILES "cyqlone/include/cyqlone/cyqlone.hpp"
          "cyqlone/include/cyqlone/cyqlone-storage.hpp"
          "cyqlone/include/cyqlone/ocp.hpp"
          "cyqlone/include/cyqlone/implementation/data.tpp"
          "cyqlone/include/cyqlone/implementation/factor.tpp"
          "cyqlone/include/cyqlone/implementation/mat-vec.tpp"
          "cyqlone/include/cyqlone/implementation/indexing.tpp"
          "cyqlone/include/cyqlone/implementation/sparse.tpp"
          "cyqlone/include/cyqlone/implementation/update.tpp"
          "cyqlone/include/cyqlone/implementation/pcg.tpp"
          "cyqlone/include/cyqlone/implementation/solve.tpp")
target_link_libraries(cyqlone PUBLIC batmat::batmat guanaqo::blas PRIVATE cyqlone::warnings)
cyqlone_configure_library(cyqlone)

# ==============================================================================

# Save the project version to a generated file
configure_file("cmake/cyqlone-version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/cyqlone-version.h" @ONLY)
target_sources(cyqlone PUBLIC FILE_SET headers TYPE HEADERS
    BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include"
    FILES "${CMAKE_CURRENT_BINARY_DIR}/include/cyqlone-version.h"
)

# Build time
if (CYQLONE_WITH_ACCURATE_BUILD_TIME)
    set(CYQLONE_BUILD_TIME_CPP "${CMAKE_CURRENT_BINARY_DIR}/cyqlone-build-time.cpp")
    add_custom_target(cyqlone-build-time-generate
        BYPRODUCTS ${CYQLONE_BUILD_TIME_CPP}
        COMMAND ${CMAKE_COMMAND}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildTime.cmake"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    target_sources(cyqlone PRIVATE ${CYQLONE_BUILD_TIME_CPP})
    add_dependencies(cyqlone cyqlone-build-time-generate)
else()
    set(CYQLONE_BUILD_TIME_CPP "${CMAKE_CURRENT_BINARY_DIR}/cyqlone-build-time.cpp")
    if (NOT EXISTS ${CYQLONE_BUILD_TIME_CPP})
        include(cmake/BuildTime.cmake)
    endif()
    target_sources(cyqlone PRIVATE ${CYQLONE_BUILD_TIME_CPP})
endif()

# Installation
include(cmake/Install.cmake)
