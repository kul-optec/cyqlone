#!/usr/bin/env bash
cd "$( dirname "${BASH_SOURCE[0]}" )"/../..
set -ex

# Select Python version
build_python_version="$(python3 --version | cut -d' ' -f2)"
python_version="${1:-${build_python_version}}"
python_majmin="$(echo "$python_version" | cut -d'.' -f1,2)"
python_majmin_nodot="${python_majmin//./}"

# Select architecture
archs=("generic")  # microarchitectures, most compatible first
triple="${2:-x86_64-bionic-linux-gnu}"
case "$triple" in
    x86_64-centos7-*) plat_tag=manylinux_2_17_x86_64; archs=("avx2" "avx512") ;;
    x86_64-bionic-*) plat_tag=manylinux_2_27_x86_64; archs=("avx2" "avx512") ;;
    aarch64-rpi3-*) plat_tag=manylinux_2_27_aarch64 ;;
    armv8-rpi3-*) plat_tag=manylinux_2_27_armv7l ;;
    armv7-neon-*) plat_tag=manylinux_2_27_armv7l ;;
    armv6-*) plat_tag=linux_armv6l ;;
    *) echo "Unknown platform ${triple}"; exit 1 ;;
esac

# Package and output directories
pkg_dir="${3:-.}"
out_dir="${4:-dist}"
with_stubs=${5:-false}

# Create Conan profile to inject the appropriate Python development files
python_profile="$PWD/conan-python.cross.profile"
profiles="$PWD/scripts/ci/conan-profiles/profiles"
cat << EOF > "$python_profile"
[options]
&:with_conan_python=True
[replace_requires]
tttapa-python-dev/*: tttapa-python-dev/[~$python_majmin, include_prerelease]
EOF

# Create a py-build-cmake configuration file for cross-compilation
pbc_config="$PWD/$triple.py-build-cmake.cross.pbc"
cat <<- EOF > "$pbc_config"
os=linux
implementation=cp
version="$python_majmin_nodot"
abi="cp$python_majmin_nodot"
arch="$plat_tag"
EOF
for i in "${!archs[@]}"; do
    c=$((i + 1))
	cat <<- EOF >> "$pbc_config"
	conan.$c.profile_host=["$profiles/toolchain/$triple.profile"]
	conan.$c.profile_host+=["$PWD/scripts/ci/profiles/${archs[$i]}.profile"]
	conan.$c.profile_host+=["$PWD/scripts/ci/profiles/unity.profile"]
	conan.$c.profile_host+=["$python_profile"]
	conan.$c.profile_host+=["$profiles/gcc-static.profile"]
	conan.$c.profile_host+=["$profiles/test/none.profile"]
	conan.$c.profile_host+=["$profiles/tools/ninja.profile"]
	conan.$c.args+=["-obatmat/*:with_openmp=True"]
	conan.$c.cmake.args+=["--fresh"]
	conan.$c.cmake.build_args+=["--verbose"]
	conan.$c.cmake.options.CYQLONE_PYTHON_POSTFIX="_${archs[$i]}"
	conan.$c.cmake.options.CMAKE_INTERPROCEDURAL_OPTIMIZATION=true
	conan.$c.cmake.install_components=["python_modules"]
	conan.$c.args+=["-o&:with_python_arch_specific_only=True"]
	EOF
done
cat <<- EOF >> "$pbc_config"
conan.1.args+=["-o&:with_python_dispatch=True"]
conan.1.args+=["-o&:with_python_arch_specific_only=False"]
conan.1.cmake.options.CYQLONE_WITH_PY_STUBS=$with_stubs
conan.1.cmake.install_components+=["python_nanobind", "python_stubs"]
EOF

# Build the Python package
python3 -m pip install -U build
python3 -m build -w "$pkg_dir" -o "$out_dir" -C cross="$pbc_config"
