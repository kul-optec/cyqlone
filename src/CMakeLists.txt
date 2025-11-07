include(cmake/Library.cmake)
find_package(guanaqo REQUIRED COMPONENTS BLAS)
find_package(batmat REQUIRED)

# Main library
# ------------------------------------------------------------------------------
add_library(cyqlone
    "cyqlone/src/cyqlone.cpp"
    "cyqlone/src/cyqlone-storage.cpp"
)
target_sources(cyqlone PUBLIC FILE_SET HEADERS
    BASE_DIRS "cyqlone/include"
    FILES "cyqlone/include/cyqlone/config.hpp"
          "cyqlone/include/cyqlone/compact.hpp"
          "cyqlone/include/cyqlone/ocp.hpp"
          "cyqlone/include/cyqlone/random-ocp.hpp"
          "cyqlone/include/cyqlone/cyqlone.hpp"
          "cyqlone/include/cyqlone/cyqlone-storage.hpp"
          "cyqlone/include/cyqlone/neumaier.hpp"
          "cyqlone/include/cyqlone/compact/elementwise.tpp"
          "cyqlone/include/cyqlone/compact/pack.tpp"
          "cyqlone/include/cyqlone/implementation/data.tpp"
          "cyqlone/include/cyqlone/implementation/factor.tpp"
          "cyqlone/include/cyqlone/implementation/indexing.tpp"
          "cyqlone/include/cyqlone/implementation/mat-vec.tpp"
          "cyqlone/include/cyqlone/implementation/pcg.tpp"
          "cyqlone/include/cyqlone/implementation/solve.tpp"
          "cyqlone/include/cyqlone/implementation/sparse.tpp"
          "cyqlone/include/cyqlone/implementation/update.tpp"
          "cyqlone/include/cyqlone/parallel.hpp"
          "cyqlone/include/cyqlone/timing.hpp"
          "cyqlone/include/cyqlone/barrier.hpp"
          "cyqlone/include/cyqlone/reduce.hpp"
)
target_link_libraries(cyqlone PUBLIC batmat::batmat guanaqo::guanaqo guanaqo::blas
                              PRIVATE cyqlone::warnings)
cyqlone_configure_library(cyqlone)
target_compile_definitions(cyqlone PUBLIC
    $<$<BOOL:${CYQLONE_COMPENSATE_SUM_LINE_SEARCH}>:CYQLONE_COMPENSATE_SUM_LINE_SEARCH>
    $<$<BOOL:${CYQLONE_QUADRUPLE_SUM_LINE_SEARCH}>:CYQLONE_QUADRUPLE_SUM_LINE_SEARCH>
    $<$<BOOL:${CYQLONE_WITH_MATIO}>:CYQLONE_WITH_MATIO>)
if (CYQLONE_WITH_MATIO)
    find_package(matio REQUIRED)
    target_sources(cyqlone PRIVATE "cyqlone/src/matio.cpp")
    target_sources(cyqlone PUBLIC FILE_SET HEADERS
        BASE_DIRS "cyqlone/include"
        FILES "cyqlone/include/cyqlone/matio.hpp")
    target_link_libraries(cyqlone PUBLIC matio::matio)
endif()
if (DEFINED CYQLONE_NAMESPACE)
    target_compile_definitions(cyqlone PUBLIC "CYQLONE_NAMESPACE=${CYQLONE_NAMESPACE}")
endif()

# QPALM solver
# ------------------------------------------------------------------------------
if (CYQLONE_WITH_QPALM)
    add_library(qpalm
        "qpalm/src/breakpoint.cpp"
        "qpalm/src/qpalm.cpp"
        "qpalm/src/status.cpp"
        "qpalm/src/ocp-backend-cyqlone.cpp"
    )
    target_sources(qpalm PUBLIC FILE_SET HEADERS
        BASE_DIRS "qpalm/include"
        FILES "qpalm/include/cyqlone/qpalm/qpalm.hpp"
              "qpalm/include/cyqlone/qpalm/status.hpp"
              "qpalm/include/cyqlone/qpalm/solver.hpp"
              "qpalm/include/cyqlone/qpalm/detailed-stats.hpp"
              "qpalm/include/cyqlone/qpalm/settings.hpp"
              "qpalm/include/cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp"
              "qpalm/include/cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp"
              "qpalm/include/cyqlone/qpalm/implementation/algorithms.hpp"
              "qpalm/include/cyqlone/qpalm/implementation/breakpoint.hpp"
              "qpalm/include/cyqlone/qpalm/implementation/breakpoint.tpp"
              "qpalm/include/cyqlone/qpalm/implementation/linesearch.tpp"
              "qpalm/include/cyqlone/qpalm/implementation/solver.tpp"
    )
    target_link_libraries(qpalm PUBLIC cyqlone::cyqlone)
    cyqlone_configure_library(qpalm EXPORT_PREFIX cyqlone)
    if (CYQLONE_WITH_SKA_SORT)
        find_package(ska-sort REQUIRED)
        target_link_libraries(qpalm PUBLIC ska-sort::ska-sort)
        target_compile_definitions(qpalm PUBLIC CYQLONE_WITH_SKA_SORT)
    endif()

    # Example problems
    if (CYQLONE_WITH_EXAMPLE_PROBLEMS)
        find_package(Eigen3 REQUIRED)
        add_library(example-problems
            "example-problems/src/conversion.cpp"
            "example-problems/src/csv.cpp"
            "example-problems/src/platooning.cpp"
            "example-problems/src/spring-mass.cpp"
        )
        target_sources(example-problems PUBLIC FILE_SET HEADERS
            BASE_DIRS "example-problems/include"
            FILES     "example-problems/include/cyqlone/qpalm/example-problems/platooning.hpp"
                      "example-problems/include/cyqlone/qpalm/example-problems/zoh.hpp"
                      "example-problems/include/cyqlone/qpalm/example-problems/csv.hpp"
                      "example-problems/include/cyqlone/qpalm/example-problems/conversion.hpp"
                      "example-problems/include/cyqlone/qpalm/example-problems/spring-mass.hpp"
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
target_sources(cyqlone PUBLIC FILE_SET HEADERS
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
