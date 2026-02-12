find_package(Doxygen QUIET)
find_package(Python3 COMPONENTS Interpreter)
if (Doxygen_FOUND AND Python3_Interpreter_FOUND)
    add_custom_target(docs-doxygen
        COMMAND Doxygen::doxygen "${CYQLONE_DOXYFILE}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation"
        USES_TERMINAL)
    add_custom_target(docs-sphinx
        COMMAND ${Python3_EXECUTABLE} -m sphinx
            "${CMAKE_CURRENT_SOURCE_DIR}/source"
            "${CMAKE_CURRENT_SOURCE_DIR}/build/html"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generating Sphinx documentation"
        USES_TERMINAL)
    add_custom_target(docs
        DEPENDS docs-doxygen docs-sphinx
        COMMENT "Generating documentation")
else()
    message(STATUS "Doxygen or Python3 interpreter not found. Skipping docs target.")
endif()
