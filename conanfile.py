import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.build import can_run
from conan.tools.files import save
from conan.tools.scm import Git


class CyqloneRecipe(ConanFile):
    name = "cyqlone"
    version = "1.0.0"

    # Optional metadata
    license = "LGPLv3"
    author = "Pieter P <pieter.p.dev@outlook.com>"
    url = "https://github.com/kul-optec/cyqlone"
    description = "Parallel solver for systems with optimal control structure."
    topics = "scientific software"

    # Binary configuration
    package_type = "library"
    settings = "os", "compiler", "build_type", "arch"
    bool_cyqlone_options = {
        "with_benchmarks": False,
        "with_blasfeo": False,
    }
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    } | {k: [True, False] for k in bool_cyqlone_options}
    default_options = {
        "shared": False,
        "fPIC": True,
    } | bool_cyqlone_options

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = (
        "CMakeLists.txt",
        "src/*",
        "cmake/*",
        "interfaces/*",
        "test/*",
        "benchmarks/*",
        "LICENSE",
        "README.md",
    )

    def export_sources(self):
        git = Git(self)
        status_cmd = "status . --short --no-branch --untracked-files=no"
        dirty = bool(git.run(status_cmd).strip())
        hash = git.get_commit() + ("-dirty" if dirty else "")
        print("Commit hash:", hash)
        save(self, os.path.join(self.export_sources_folder, "commit.txt"), hash)

    generators = ("CMakeDeps",)

    def requirements(self):
        self.requires("guanaqo/1.0.0-alpha.15", transitive_headers=True, transitive_libs=True, force=True)
        self.requires("batmat/1.0.0", transitive_headers=True, transitive_libs=True, force=True)
        if self.options.get_safe("with_blasfeo"):
            self.requires("blasfeo/0.1.4.1")
        if self.options.get_safe("with_benchmarks"):
            self.requires("benchmark/1.8.4")
            self.requires("hyhound/1.0.0")
        self.test_requires("gtest/1.15.0")

    def config_options(self):
        if self.settings.get_safe("os") == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        # There is currently no 64-bit indices option for OpenBLAS using Conan
        if not self.options.get_safe("with_benchmarks"):
            self.options.rm_safe("with_mkl")
            self.options.rm_safe("with_openblas")
        if self.options.get_safe("with_openblas"):
            self.options.rm_safe("dense_index_type")
        self.options["guanaqo/*"].with_blas = True
        self.options["hyhound/*"].with_ocp = True

    def layout(self):
        cmake_layout(self)
        self.cpp.build.builddirs.append("")

    def generate(self):
        tc = CMakeToolchain(self)
        index_t = self.options.get_safe("dense_index_type", default="int")
        tc.variables["CYQLONE_DENSE_INDEX_TYPE"] = index_t
        for k in self.bool_cyqlone_options:
            value = getattr(self.options, k, None)
            if value is not None and value.value is not None:
                tc.variables["CYQLONE_" + k.upper()] = bool(value)
        if can_run(self):
            tc.variables["CYQLONE_FORCE_TEST_DISCOVERY"] = True
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs.append(os.path.join("lib", "cmake", "cyqlone"))
