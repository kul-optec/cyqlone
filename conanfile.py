import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.build import can_run
from conan.tools.files import save
from conan.tools.scm import Git


class CyqloneRecipe(ConanFile):
    name = "cyqlone"
    version = "0.0.1"

    # Optional metadata
    license = "LGPL-3.0-or-later"
    author = "Pieter P <pieter.p.dev@outlook.com>"
    url = "https://github.com/kul-optec/cyqlone"
    description = "Parallel solver for systems with optimal control structure."
    topics = "scientific software"

    # Binary configuration
    package_type = "library"
    settings = "os", "compiler", "build_type", "arch"
    # https://github.com/conan-io/conan/issues/19108
    package_id_non_embed_mode = "full_mode"
    bool_cyqlone_options = {
        "with_benchmarks": False,
        "with_examples": True,
        "with_qpalm": True,
        "with_example_problems": True,
        "with_blasfeo": False,
        "with_python": False,
        "with_python_dispatch": False,
        "with_python_arch_specific_only": False,
        "with_ska_sort": False,
        "with_matio": True,
        "with_zlib": True,
    }
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_conan_python": [True, False],
    } | {k: [True, False] for k in bool_cyqlone_options}
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_conan_python": False,
    } | bool_cyqlone_options

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = (
        "CMakeLists.txt",
        "src/*",
        "cmake/*",
        "interfaces/*",
        "test/*",
        "examples/*",
        "benchmarks/*",
        "!benchmarks/cyqpalm/*",
        "LICENSE",
        "README.md",
    )

    def export_sources(self):
        if os.path.exists(os.path.join(self.recipe_folder, ".git")):
            git = Git(self)
            status_cmd = "status . --short --no-branch --untracked-files=no"
            dirty = bool(git.run(status_cmd).strip())
            hash = git.get_commit() + ("-dirty" if dirty else "")
            print("Commit hash:", hash)
            save(self, os.path.join(self.export_sources_folder, "commit.txt"), hash)

    generators = ("CMakeDeps",)

    def requirements(self):
        self.requires("guanaqo/1.0.0-alpha.25", transitive_headers=True, transitive_libs=True)
        self.requires("batmat/0.0.18", transitive_headers=True, transitive_libs=True)
        if self.options.get_safe("with_zlib"):
            self.requires("zlib/[~1]")
        if self.options.get_safe("with_python"):
            self.requires("nanobind/2.10.2")
            if self.options.with_python_dispatch:
                self.requires("cpu_features/0.10.1")
            if self.options.with_conan_python:
                self.requires("tttapa-python-dev/3.13.7")
        if self.options.get_safe("with_python") or self.options.get_safe("with_example_problems"):
            self.requires("eigen/[~3.4 || ~5.0]", transitive_headers=True)
        else:
            self.test_requires("eigen/[~3.4 || ~5.0]")
        if self.options.get_safe("with_blasfeo"):
            self.requires("blasfeo/tttapa.20260119")
        if self.options.get_safe("with_ska_sort"):
            self.requires("ska-sort/tttapa.20250919")
        if self.options.get_safe("with_matio"):
            self.requires("matio/1.5.27", transitive_headers=True)
        if self.options.get_safe("with_benchmarks"):
            self.requires("benchmark/1.9.4")
            self.requires("hyhound/1.1.1")
        self.test_requires("gtest/1.17.0")

    def config_options(self):
        if self.settings.get_safe("os") == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if not self.options.get_safe("with_qpalm"):
            self.options.rm_safe("with_example_problems")
            self.options.rm_safe("with_python")
        if not self.options.get_safe("with_python"):
            self.options.rm_safe("with_python_dispatch")
            self.options.rm_safe("with_python_arch_specific_only")
            self.options.rm_safe("with_conan_python")
        self.options["guanaqo/*"].with_blas = True
        self.options["hyhound/*"].with_ocp = True
        self.options["matio/*"].with_hdf5 = False
        self.options["matio/*"].mat73 = False

    def layout(self):
        if self.folders.build_folder_vars is None:
            if self.options.get_safe("with_python"):
                self.folders.build_folder_vars = ["const.python"]
        cmake_layout(self)
        self.cpp.build.builddirs.append("")

    def generate(self):
        tc = CMakeToolchain(self)
        batmat = self.dependencies["batmat"]
        guanaqo = self.dependencies["guanaqo"]
        for k in self.bool_cyqlone_options:
            value = self.options.get_safe(k, None)
            if value is not None and value.value is not None:
                tc.cache_variables["CYQLONE_" + k.upper()] = bool(value)
        if can_run(self):
            tc.cache_variables["CYQLONE_FORCE_TEST_DISCOVERY"] = True
            if self.options.with_python:
                tc.cache_variables["CYQLONE_WITH_PY_STUBS"] = True
        tc.variables["CYQLONE_DOCS_GUANAQO_VERSION"] = str(guanaqo.ref.version)
        tc.variables["CYQLONE_DOCS_BATMAT_VERSION"] = str(batmat.ref.version)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        if self.options.with_python:
            cmake.install(component="python_source")
            cmake.install(component="python_modules")
            cmake.install(component="python_stubs")

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs.append(os.path.join("lib", "cmake", "cyqlone"))
