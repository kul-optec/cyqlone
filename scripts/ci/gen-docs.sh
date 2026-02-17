#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
repodir="$PWD"/../..
set -x
cd "$repodir"

mainbranch="main"
output_folder="${1:-/tmp}"
conf_preset="${2:-conan-debug}"
build_preset="${3:-${conf_preset}}"
mkdir -p "$output_folder"

# Function that builds the doxygen documentation.
# usage:    run_doxygen <branch-name> <output-directory>
function run_doxygen {
    branch="$1"
    if [ "$branch" = "$mainbranch" ]; then
        outdir="$2"
    else
        outdir="$2/$branch"
    fi
    htmldir="Doxygen"
    # Remove the old documentation
    rm -rf "$outdir/$htmldir"

    # Set conf_preset to "clean" to just clean the old documentation
    if [ "$conf_preset" = "clean" ]; then return; fi

    # Configure the project
    cmake --fresh --preset "$conf_preset" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=On \
        -DCYQLONE_WITH_COVERAGE=On \
        -DCYQLONE_FORCE_TEST_DISCOVERY=On \
        -DDOXYGEN_HTML_OUTPUT="$htmldir" \
        -DDOXYGEN_OUTPUT_DIRECTORY="$outdir" \
        -DDOXYGEN_PROJECT_NUMBER="$branch"

    # Generate the Doxygen C++ documentation
    cmake --build --preset "$build_preset" -t docs
}

# Generate the documentation for the current branch
git fetch ||:
curr_branch=$(git branch --show-current)
if [ -n "$curr_branch" ]; then
    run_doxygen "$curr_branch" "$output_folder"
elif [ -n "$CI_COMMIT_BRANCH" ]; then
    run_doxygen "$CI_COMMIT_BRANCH" "$output_folder"
fi
# Generate the documentation for the current tag
git fetch --tags ||:
if curr_tag=$(git describe --tags --exact-match); then
    run_doxygen "$curr_tag" "$output_folder"
fi

set +x

echo "Done generating documentation"
