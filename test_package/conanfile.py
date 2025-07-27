import glob
import os
from io import StringIO

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run
from conan.tools.env import Environment


class LassTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        # Check test_module has the expected extension according to Limited API
        test_module = [
            path
            for path in glob.glob(os.path.join(self.cpp.build.bindir, "test_module.*"))
            if path.endswith((".pyd", ".so"))
        ]
        assert len(test_module) == 1, test_module
        out = StringIO()
        self.run(
            'python -c "import sysconfig; '
            "print(sysconfig.get_config_var('EXT_SUFFIX') or '')\"",
            stdout=out,
            env="conanrun",
        )
        ext_suffix = out.getvalue().strip()
        assert ext_suffix
        has_limited_api = bool(
            self.dependencies["lass"].options.get_safe("py_limited_api")
        )
        assert test_module[0].endswith(ext_suffix) == (not has_limited_api)

        # Run the test executable
        if can_run(self):
            env = Environment()
            env.prepend_path("PYTHONPATH", self.cpp.build.bindir)
            with env.vars(self).apply():
                cmd = os.path.join(self.cpp.build.bindir, "test_package")
                self.run(cmd, env="conanrun")
