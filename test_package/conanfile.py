import os

# pyright: reportMissingImports=false
from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.cmake import CMake, cmake_layout


class LassTestConan(ConanFile):  # type: ignore[misc]
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self) -> None:
        self.requires(self.tested_reference_str)

    def build(self) -> None:
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self) -> None:
        cmake_layout(self)

    def test(self) -> None:
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "test_package")
            self.run(cmd, env="conanrun")
