find_package(Doxygen OPTIONAL_COMPONENTS dot QUIET)
find_package(Python3 COMPONENTS Interpreter)

option(CYQLONE_DOCS_DOWNLOAD_TAGFILES
    "Whether to download tag files for external documentation" On)
set(CYQLONE_DOCS_GUANAQO_VERSION "1.0.0-alpha.23" CACHE STRING
    "Version of guanaqo to use for documentation")
set(CYQLONE_DOCS_GUANAQO_URL "https://tttapa.github.io/guanaqo" CACHE STRING
    "URL of guanaqo documentation")

# Get the git tag and branch, if available, to use for the documentation version.
function(cyqlone_get_git_ref result_tag result_branch)
    execute_process(  # Try to get exact tag at HEAD
        COMMAND git describe --exact-match --tags
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TAG
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_TAG_RESULT
    )
    if(GIT_TAG_RESULT EQUAL 0)
        set(${result_tag} "${GIT_TAG}" PARENT_SCOPE)
    else()
        unset(${result_tag} PARENT_SCOPE)
    endif()
    execute_process(  # Try to get the branch name
        COMMAND git symbolic-ref --short HEAD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_BRANCH_RESULT
    )
    if(GIT_BRANCH_RESULT EQUAL 0)
        set(${result_branch} "${GIT_BRANCH}" PARENT_SCOPE)
    else()
        unset(${result_branch} PARENT_SCOPE)
    endif()
endfunction()

# Download tag files for external documentation if enabled.
function(cyqlone_get_tag_files result)
    if (NOT CYQLONE_DOCS_DOWNLOAD_TAGFILES)
        unset(${result} PARENT_SCOPE)
        return()
    endif()
    set(tag_files "")
    set(guanaqo_url "${CYQLONE_DOCS_GUANAQO_URL}/${CYQLONE_DOCS_GUANAQO_VERSION}/Doxygen")
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/guanaqo.tag")
        file(DOWNLOAD "${guanaqo_url}/guanaqo.tag" "${CMAKE_CURRENT_BINARY_DIR}/guanaqo.tag"
            STATUS download_status)
        if (NOT download_status EQUAL 0)
            message(WARNING "Failed to download guanaqo.tag from ${guanaqo_url}")
        endif()
    endif()
    if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/guanaqo.tag")
        list(APPEND tag_files "${CMAKE_CURRENT_BINARY_DIR}/guanaqo.tag = ${guanaqo_url}")
    endif()
    set(${result} "${tag_files}" PARENT_SCOPE)
endfunction()

if (Doxygen_FOUND AND Python3_Interpreter_FOUND)
    include(Doxyfile.cmake)
    cyqlone_get_git_ref(GIT_TAG GIT_BRANCH)
    if (GIT_TAG)
        set(DOXYGEN_PROJECT_NUMBER "${GIT_TAG}")
    elseif (GIT_BRANCH)
        set(DOXYGEN_PROJECT_NUMBER "${GIT_BRANCH}")
    else()
        set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}")
    endif()
    cyqlone_get_tag_files(DOXYGEN_TAGFILES)
    add_custom_target(docs-output-dir
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DOXYGEN_OUTPUT_DIRECTORY}"
        COMMENT "Creating Doxygen output directory")
    doxygen_add_docs(
        docs-doxygen
            "${PROJECT_SOURCE_DIR}/src"
            "${PROJECT_SOURCE_DIR}/docs/source/doxygen/groups.dox"
            "${PROJECT_SOURCE_DIR}/docs/source/doxygen/examples.md"
            "${PROJECT_SOURCE_DIR}/README.md"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation")
    add_dependencies(docs-doxygen docs-output-dir)
    add_custom_target(docs-sphinx
        COMMAND ${Python3_EXECUTABLE} -m sphinx
            "${CMAKE_CURRENT_SOURCE_DIR}/source"
            "${DOXYGEN_OUTPUT_DIRECTORY}/Sphinx"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generating Sphinx documentation"
        USES_TERMINAL)
    add_dependencies(docs-sphinx docs-doxygen)
    add_custom_target(docs DEPENDS docs-sphinx)
else()
    message(STATUS "Doxygen or Python3 interpreter not found. Skipping docs target.")
endif()
