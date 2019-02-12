import os
import errno
import subprocess
import re

from conans import ConanFile, CMake, tools, errors


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

    # we want to add a prerelease number to the semver. We can use describe
    # to get an increasing number from a previous tag. To get a stable number
    # we need to fixe that base tag, which is obviously not the current version
    # since that tag doesn't exist yet.
    # NOTE: you can only update this base tag _at_the_same_time_ you update
    # project(Lass VERSION ...) in CMakeLists. Otherwise, the prerelease
    # numbers will not properly increase.
    basetag = "lass-1.6.0"

    git = tools.Git()
    try:
        describe = git.run(
            "describe --abbrev=8 --dirty --match {}".format(basetag))
    except errors.ConanException:
        return None  # Assume conan metadata already knows
    assert describe.startswith(basetag), \
        "{!r} does not start with {!r}".format(describe, basetag)
    match = re.search(r"{}-(\d+)-(g\w+(-dirty)?)$".format(re.escape(basetag)),
                      describe)
    assert match, "unexpected describe format: {!r}".format(describe)
    pre_release, rev = match.group(1), match.group(2)
    return "{}-{}+{}".format(version, pre_release, rev)


class LassConan(ConanFile):
    name = "Lass"
    version = _get_version()
    license = "CPAL-1.0", "GPL-2.0-or-later"
    author = "Cocamware <info@cocamware.com>"
    url = "https://github.com/cocamware/lass.git"
    description = "Library of Assembled Shared Source."
    topics = "C++", "Python"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "simd_aligned": [True, False],
        "without_iterator_debugging": [True, False],
        "with_std_auto_ptr": [True, False, None],
        "python_executable": "ANY",
        "python_version": "ANY",
        "python_debug": [True, False, None],
    }
    default_options = {
        "shared": True,
        "simd_aligned": False,
        "without_iterator_debugging": False,
        "with_std_auto_ptr": None,
        "python_executable": "python",
        "python_version": None,
        "python_debug": None,
    }
    generators = "cmake"
    exports_sources = ("*", "!build*", "!env*", "!venv*")
    build_requires = ("cmake_installer/[>=3.1]@conan/stable")

    def configure(self):
        python = Python(self.options.python_executable, self.settings)
        if not self.options.python_version:
            self.options.python_version = python.version
        if self.options.python_debug.value in (None, 'None'):
            self.options.python_debug = python.debug

    def _cmake(self):
        # Delay final checks and queries involving options.python_executable
        # until here, since potential build_requires can influence PATH,
        # and that PATH is yet not in effect in configure nor in package_id.
        # Exception in configure is when python_version and python_debug are
        # not configured.

        # double check python_version and python_debug
        python = Python(self.options.python_executable, self.settings)
        if str(self.options.python_version) != python.version:
            raise errors.ConanInvalidConfiguration(
                "python_version option not compatible with python_executable:"
                "{} != {}".format(
                    str(self.options.python_version), python.version))
        if self.options.python_debug != python.debug:
            raise errors.ConanInvalidConfiguration(
                "python_debug option not compatible with python_executable.")
        if self.settings.os == "Windows":
            if python.debug and self.settings.build_type != "Debug":
                raise errors.ConanInvalidConfiguration(
                    "Can't build non-debug Lass with debug Python on Windows.")

        cmake = CMake(self)

        # pylint: disable=invalid-name
        if cmake.get_version() < "3.12":
            Python_EXECUTABLE = "PYTHON_EXECUTABLE"
            Python_LIBRARY_RELEASE = "PYTHON_LIBRARY"
            Python_LIBRARY_DEBUG = "PYTHON_DEBUG_LIBRARY"
        else:
            Python_EXECUTABLE = "Python_EXECUTABLE"
            Python_LIBRARY_RELEASE = "Python_LIBRARY_RELEASE"
            Python_LIBRARY_DEBUG = "Python_LIBRARY_DEBUG"

        defs = {
            "BUILD_TESTING": self.develop and not self.in_local_cache,
            "BUILD_SIMD_ALIGNED": bool(self.options.simd_aligned),
            "BUILD_WITHOUT_ITERATOR_DEBUGGING": \
                bool(self.options.without_iterator_debugging),
            "Lass_PYTHON_VERSION": python.version,
            Python_EXECUTABLE: python.executable,
            Python_LIBRARY_RELEASE: python.library,
            Python_LIBRARY_DEBUG: python.library,
        }
        if self.options.with_std_auto_ptr.value not in (None, "None"):
            defs["LASS_HAVE_STD_AUTO_PTR"] = \
                bool(self.options.with_std_auto_ptr)

        cmake.configure(source_folder=".", defs=defs)
        return cmake

    def build(self):
        cmake = self._cmake()
        cmake.configure(source_folder=".")
        cmake.build()

    def package(self):
        cmake = self._cmake()
        cmake.install()

    def package_info(self):
        lass_config = self._lass_config()
        if self.settings.build_type == "Debug":
            libs = [
                lass_config.LASS_OUTPUT_NAME_DEBUG,
                lass_config.LASS_PYTHON_OUTPUT_NAME_DEBUG
            ]
        else:
            libs = [
                lass_config.LASS_OUTPUT_NAME,
                lass_config.LASS_PYTHON_OUTPUT_NAME
            ]
        self.cpp_info.libs = [lib for lib in libs if lib]
        self.cpp_info.cppflags = [lass_config.LASS_EXTRA_CXX_FLAGS]

    def _lass_config(self):
        path = os.path.join(self.cpp_info.rootpath, "share", "Lass",
                            "LassConfig.py")
        import imp
        imp.acquire_lock()
        try:
            return imp.load_source("LassConfig", path)
        finally:
            imp.release_lock()


class Python(object):
    """Build and config info of a Python"""

    def __init__(self, executable, settings):
        self._executable = tools.which(str(executable))
        self._os = settings.os

    @property
    def executable(self):
        """Full path to Python executable"""
        return self._executable

    @property
    def version(self):
        """Short version like 3.7"""
        # pylint: disable=attribute-defined-outside-init
        try:
            return self._version
        except AttributeError:
            self._version = self._get_config_var('py_version_short')
            return self._version

    @property
    def debug(self):
        """True if python library is built with Py_DEBUG"""
        # pylint: disable=attribute-defined-outside-init
        try:
            return self._debug
        except AttributeError:
            output = self._query(
                "import sys; print(int(hasattr(sys, 'gettotalrefcount')))")
            self._debug = bool(int(output))
            return self._debug

    @property
    def library(self):
        """Full path to python library you should link to"""
        # pylint: disable=attribute-defined-outside-init
        try:
            return self._library
        except AttributeError:
            if self._os == "Windows":
                stdlib = self._get_python_path("stdlib")
                version_nodot = self._get_config_var("py_version_nodot")
                postfix = "_d" if self.debug else ""
                self._library = os.path.join(
                    os.path.dirname(stdlib), "libs", "python{}{}.lib".format(
                        version_nodot, postfix))
            else:
                fname = self._get_config_var('LDLIBRARY')
                for dir_key in ("LIBDIR", "LIBPL"):
                    dirname = self._get_config_var(dir_key)
                    candidate = os.path.join(dirname, fname)
                    if os.path.isfile(candidate):
                        self._library = candidate
                        break
                else:
                    raise RuntimeError("Unable to find {}".format(fname))
            return self._library

    def _get_python_path(self, key):
        return self._query(
            "import sysconfig; print(sysconfig.get_path({!r}))".format(key))

    def _get_config_var(self, key):
        return self._query("import sysconfig; "
                           "print(sysconfig.get_config_var({!r}))".format(key))

    def _query(self, script):
        return subprocess.check_output([self.executable, "-c", script],
                                       universal_newlines=True).strip()
