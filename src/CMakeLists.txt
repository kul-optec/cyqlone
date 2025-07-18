include(cmake/Library.cmake)
find_package(guanaqo REQUIRED COMPONENTS BLAS)
find_package(batmat REQUIRED)

# Main library
# ------------------------------------------------------------------------------
add_library(cyqlone
    "cyqlone/src/cyqlone.cpp"
    "cyqlone/src/cyqlone-storage.cpp"
)
target_sources(cyqlone PUBLIC FILE_SET headers TYPE HEADERS
    BASE_DIRS "cyqlone/include"
    FILES "cyqlone/include/cyqlone/config.hpp"
          "cyqlone/include/cyqlone/cyqlone.hpp"
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
target_link_libraries(cyqlone PUBLIC batmat::batmat guanaqo::guanaqo guanaqo::blas PRIVATE cyqlone::warnings)
cyqlone_configure_library(cyqlone)

# QPALM solver
# ------------------------------------------------------------------------------
if (CYQLONE_WITH_QPALM)
    add_library(qpalm
        "qpalm/src/qpalm.cpp"
        "qpalm/src/status.cpp"
        "qpalm/src/ocp-backend-cyclic.cpp"
    )
    target_sources(qpalm PUBLIC FILE_SET HEADERS
        BASE_DIRS "qpalm/include"
        FILES "qpalm/include/cyqlone/qpalm/qpalm.hpp"
              "qpalm/include/cyqlone/qpalm/status.hpp"
              "qpalm/include/cyqlone/qpalm/solver.hpp"
              "qpalm/include/cyqlone/qpalm/detailed-stats.hpp"
              "qpalm/include/cyqlone/qpalm/backends/ocp-backend-cyclic.hpp"
              "qpalm/include/cyqlone/qpalm/settings.hpp"
              "qpalm/include/cyqlone/qpalm/backends/ocp-backend-cyclic.tpp"
              "qpalm/include/cyqlone/qpalm/implementation/linesearch.tpp"
              "qpalm/include/cyqlone/qpalm/implementation/solver.tpp"
    )
    target_link_libraries(qpalm PUBLIC cyqlone::cyqlone)
    cyqlone_configure_library(qpalm EXPORT_PREFIX cyqlone)

    # LADEL backends
    if (CYQLONE_WITH_LADEL)
        find_package(LADEL REQUIRED)
        find_package(Eigen3 REQUIRED)
        add_library(ladel-backend
            "qpalm/src/ladel-schur-backend.cpp"
        )
        target_sources(ladel-backend PUBLIC FILE_SET HEADERS
            BASE_DIRS "qpalm/include"
            FILES "qpalm/include/cyqlone/qpalm/backends/ladel/ladel-schur-backend.tpp"
        )
        target_link_libraries(ladel-backend PUBLIC qpalm PRIVATE LADEL::ladel Eigen3::Eigen)
        cyqlone_qpalm_configure_library(ladel-backend EXPORT_PREFIX cyqlone)
    endif()

    # Example problems
    if (CYQLONE_WITH_EXAMPLE_PROBLEMS)
        find_package(Eigen3 REQUIRED)
        add_library(example-problems
            "example-problems/src/conversion.cpp"
            "example-problems/src/csv.cpp"
            "example-problems/src/platooning.cpp"
        )
        target_sources(example-problems PUBLIC FILE_SET HEADERS
            BASE_DIRS "example-problems/include"
            FILES     "example-problems/include/cyqlone/qpalm/example-problems/platooning.hpp"
                      "example-problems/include/cyqlone/qpalm/example-problems/zoh.hpp"
                      "example-problems/include/cyqlone/qpalm/example-problems/csv.hpp"
                      "example-problems/include/cyqlone/qpalm/example-problems/conversion.hpp"
        )
        target_link_libraries(example-problems
            PUBLIC cyqlone::cyqlone Eigen3::Eigen guanaqo::guanaqo guanaqo::blas)
        cyqlone_configure_library(example-problems EXPORT_PREFIX cyqlone)
    endif()
endif()

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
