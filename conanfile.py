from conans import ConanFile, CMake, tools, errors
import errno
import re
import os
import subprocess


def _get_version():
    try:
        content = tools.load("CMakeLists.txt")
    except EnvironmentError as err:
        if err.errno == errno.ENOENT:
            return None  # Assume conan metadata already knows
        raise
    match = re.search(r"project\(Lass VERSION (\d+\.\d+\.\d+)\)", content)
    assert match
    version = match.group(1)

    git = tools.Git()
    try:
        describe = git.run("describe --tags")
    except errors.ConanException:
        return None  # Assume conan metadata already knows
    match = re.search(r"{}-(\d+)-(g\w+)$".format(re.escape(version)), describe)
    if not match:
        rev = git.get_revision()  # should work since describe did.
        return "{}+g{}".format(version, rev[:7])
    num_commits, rev = match.group(1), match.group(2)
    return "{}-{}+{}".format(version, num_commits, rev)


class LassConan(ConanFile):
    name = "Lass"
    version = _get_version()
    license = "CPAL-1.0", "GPL-2.0-or-later"
    author = "Cocamware <info@cocamware.com>"
    url = "https://git.code.sf.net/p/lass/lass.git"
    description = "Library of Assembled Shared Source."
    topics = "C++", "Python"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "simd_aligned": [True, False],
        "no_iterator_debugging": [True, False],
        "python": "ANY",
    }
    default_options = {
        "shared": True,
        "simd_aligned": False,
        "no_iterator_debugging": False,
        "python": "python",
    }
    generators = "cmake"
    exports_sources = ("*", "!build*", "!env*", "!venv*")

    def _configure(self):
        cmake = CMake(self)
        cmake.configure(
            source_folder=".",
            defs={
                "BUILD_TESTING": False,
                "BUILD_SIMD_ALIGNED": self.options.simd_aligned,
                "BUILD_WITHOUT_ITERATOR_DEBUGGING": \
                    self.options.no_iterator_debugging,
                "Lass_PYTHON_VERSION": self._python_version(),
                "Python_EXECUTABLE": self._python_executable(),
            })
        return cmake

    def build(self):
        cmake = self._configure()
        cmake.configure(source_folder=".")
        cmake.build()

    def package(self):
        cmake = self._configure()
        cmake.install()

    def package_info(self):
        lass_config = self._lass_config()
        self.cpp_info.libs = [lass_config.LASS_OUTPUT_NAME]
        if lass_config.LASS_PYTHON_OUTPUT_NAME:
            self.cpp_info.libs += [lass_config.LASS_PYTHON_OUTPUT_NAME]
        self.cpp_info.cppflags = [lass_config.LASS_EXTRA_CXX_FLAGS]

    def package_id(self):
        # pylint: disable=no-member
        self.info.options.python_version = self._python_version()

    def _lass_config(self):
        path = os.path.join(self.cpp_info.rootpath, "lib", "LassConfig.py")
        import imp
        imp.acquire_lock()
        try:
            return imp.load_source("LassConfig", path)
        finally:
            imp.release_lock()
        path = os.path.join(self.cpp_info.rootpath, "lib", "LassConfig.py")
        import imp
        imp.acquire_lock()
        try:
            return imp.load_source("LassConfig", path)
        finally:
            imp.release_lock()

    def _python_version(self):
        try:
            return self.__python_version
        except AttributeError:
            cmd = [
                self._python_executable(), "-c", "import sys;"
                "sys.stdout.write('.'.join(map(str, sys.version_info[:2])))"
            ]
            self.__python_version = subprocess.check_output(
                cmd, universal_newlines=True).strip()
            return self.__python_version

    def _python_executable(self):
        try:
            return self.__python_executable
        except AttributeError:
            cmd = [
                str(self.options.python), "-c",
                "import sys; sys.stdout.write(sys.executable)"
            ]
            self.__python_executable = subprocess.check_output(
                cmd, universal_newlines=True).strip()
            return self.__python_executable
