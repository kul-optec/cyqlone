#!/usr/bin/env bash
cd "$( dirname "${BASH_SOURCE[0]}" )"/../..
set -ex

# Package and output directories
pkg_dir="${1:-.}"
out_dir="${2:-dist}"
install_stubs_dir="$3"

# Create a py-build-cmake config file
pbc_config="$PWD/native-py-build-cmake.local.pbc"
profiles="$PWD/scripts/ci/conan-profiles/profiles"
cat << EOF > "$pbc_config"
conan.profile_host=["default"]
conan.profile_host+=["$profiles/color/gcc.profile"]
conan.profile_host+=["$profiles/link/lto-auto.profile"]
conan.profile_host+=["$profiles/visibility/hidden.profile"]
conan.profile_host+=["$profiles/sccache/only-self.profile"]
conan.profile_host+=["$profiles/test/none.profile"]
conan.profile_host+=["$profiles/tools/ninja.profile"]
conan.args+=["-obatmat/*:with_openmp=True"]
conan.cmake.args+=["--fresh"]
conan.cmake.build_args+=["--verbose"]
conan.cmake.options.CYQLONE_WITH_PY_STUBS=true
EOF

# Build the Python package
python3 -m pip install -U build
python3 -m build -w "$pkg_dir" -o "$out_dir" -C local="$pbc_config"

# Install the Python stubs
if [ -n "$install_stubs_dir" ]; then
    # Install py-build-cmake
    python3 -m pip install 'py-build-cmake~=0.6.0a2' 'numpy<3'
    # Determine Conan's build directory
    pbc=(python3 -m py_build_cmake.cli -C "$pkg_dir" --local="$pbc_config")
    build_config="$("${pbc[@]}" build-config-name)"
    build_dir="$pkg_dir/.py-build-cmake_cache/build/$build_config/Release"
    # Activate the Conan build environment (ensures that CMake is in PATH)
    set +x; source "$build_dir/generators/conanbuild.sh"; set -x
    # Re-run CMake to change Python executable (old one is in a temporary venv)
    cmake "$build_dir" -D "Python_EXECUTABLE=$(which python3)"
    # Install the binary modules into the source tree
    cmake --install "$build_dir" --prefix "$install_stubs_dir" \
        --component python_modules
    cmake --install "$build_dir" --prefix "$install_stubs_dir" \
        --component python_nanobind
    # Generate the stubs (using a complete tree including the binary modules)
    cmake --install "$build_dir" --prefix "$install_stubs_dir" \
        --component python_stubs
    # Then we remove the binary modules again (sdist is source only)
    while IFS= read -r f || [ -n "$f" ]; do rm -f "$f"
    done < "$build_dir/install_manifest_python_modules.txt"
    while IFS= read -r f || [ -n "$f" ]; do rm -f "$f"
    done < "$build_dir/install_manifest_python_nanobind.txt"
fi
