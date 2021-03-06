import errno
import os
import re
import subprocess

from conans import CMake, ConanFile, errors, tools


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

    # if this version has been tagged in Git, it's either this very commit (and a
    # release version), or a post-release (which is not allowed).
    version_tag = "lass-{}".format(version)
    try:
        describe = git.run(
            "describe --abbrev=8 --dirty --tags --match {}".format(version_tag)
        )
    except (errors.ConanException, errors.CalledProcessErrorWithStderr):
        pass  # release tag doesn't exist yet, it's a pre-release.
    else:
        assert describe == version_tag, (
            "Post-release builds are not allowed. Set base_tag to {!r} and "
            + "increment project(Lass VERSION X.Y.Z) in CMakeLists.txt."
        ).format(version_tag)
        return version

    # It's not a release version, so we want to add a pre-release number to the semver.
    # We can use describe to get an increasing number from a previous tag. To get a
    # stable number we need to fix that base tag, which is obviously not the current
    # version since that tag doesn't exist yet.
    # NOTE: you can only update this base tag _at_the_same_time_ you update
    # project(Lass VERSION ...) in CMakeLists. Otherwise, the prerelease numbers will
    # not properly increase.
    base_tag = "lass-1.10.0"

    git = tools.Git()
    try:
        describe = git.run(
            "describe --abbrev=8 --dirty --tags --match {}".format(base_tag)
        )
    except errors.ConanException:
        return None  # Assume conan metadata already knows
    assert describe.startswith(base_tag), "{!r} does not start with {!r}".format(
        describe, base_tag
    )
    match = re.search(
        r"{}-(\d+)-(g\w+(-dirty)?)$".format(re.escape(base_tag)), describe
    )
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
        "fPIC": [True, False],
        "simd_aligned": [True, False],
        "without_iterator_debugging": [True, False],
        "with_std_auto_ptr": [True, False, None],
        "python_executable": "ANY",
        "python_version": "ANY",
        "python_debug": [True, False, None],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "simd_aligned": False,
        "without_iterator_debugging": False,
        "with_std_auto_ptr": None,
        "python_executable": "python",
        "python_version": None,
        "python_debug": None,
    }
    generators = "cmake"
    exports_sources = ("*", "!build*", "!env*", "!venv*", "!dist*", "!pylass.egg-info")

    def config_options(self):
        if self.settings.compiler == "Visual Studio":
            del self.options.fPIC

    def configure(self):
        # Do *not* rely on any checks or queries involving
        # options.python_executable in configure. Potential build requirements
        # can influence PATH, and that's no in effect in configure nor in
        # package_id.
        #
        # Exception is when python_version and python_debug are not explicitly
        # configured, because this is your last change to do so.
        #
        # This means, if you rely on python to be found on a build requirement's
        # PATH, you must be explicit about python_version and python_debug.
        #
        if not self.options.python_version:
            self.options.python_version = Python(
                self.options.python_executable, self.settings
            ).version
        if self.options.python_debug.value in (None, "None"):
            self.options.python_debug = Python(
                self.options.python_executable, self.settings
            ).debug

    def _cmake(self):
        python = Python(self.options.python_executable, self.settings)
        if str(self.options.python_version) != python.version:
            raise errors.ConanInvalidConfiguration(
                "python_version option not compatible with python_executable:"
                "{} != {}".format(str(self.options.python_version), python.version)
            )
        if self.options.python_debug != python.debug:
            raise errors.ConanInvalidConfiguration(
                "python_debug option not compatible with python_executable."
            )
        if self.settings.os == "Windows":
            if python.debug and self.settings.build_type != "Debug":
                raise errors.ConanInvalidConfiguration(
                    "Can't build non-debug Lass with debug Python on Windows."
                )

        cmake = CMake(self)
        defs = {
            "BUILD_TESTING": self.develop and not self.in_local_cache,
            "BUILD_SIMD_ALIGNED": bool(self.options.simd_aligned),
            "BUILD_WITHOUT_ITERATOR_DEBUGGING": bool(
                self.options.without_iterator_debugging
            ),
            "Lass_PYTHON_VERSION": python.version,
            "Python_EXECUTABLE": python.executable,
            "Python_LIBRARY_RELEASE": python.library,
            "Python_LIBRARY_DEBUG": python.library,
        }
        if self.options.with_std_auto_ptr.value not in (None, "None"):
            defs["LASS_HAVE_STD_AUTO_PTR"] = bool(self.options.with_std_auto_ptr)

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
                lass_config.LASS_PYTHON_OUTPUT_NAME_DEBUG,
            ]
        else:
            libs = [
                lass_config.LASS_OUTPUT_NAME,
                lass_config.LASS_PYTHON_OUTPUT_NAME,
            ]
        self.cpp_info.libs = [lib for lib in libs if lib]
        self.cpp_info.cxxflags = lass_config.LASS_EXTRA_CXX_FLAGS

    def _lass_config(self):
        module_name = "LassConfig"
        file_path = os.path.join(
            self.cpp_info.rootpath, "share", "Lass", "LassConfig.py"
        )
        try:
            from importlib.util import spec_from_file_location, module_from_spec
        except ImportError:
            import imp

            imp.acquire_lock()
            try:
                return imp.load_source(module_name, file_path)
            finally:
                imp.release_lock()
        else:
            spec = spec_from_file_location(module_name, file_path)
            module = module_from_spec(spec)
            spec.loader.exec_module(module)
            return module


class Python(object):
    """Build and config info of a Python"""

    def __init__(self, executable, settings):
        self._executable = tools.which(str(executable))
        self._os = settings.os

        if not self._executable:
            raise errors.ConanInvalidConfiguration(
                "Python executable cannot be found: {!s}".format(self._executable)
            )

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
            self._version = self._get_config_var("py_version_short")
            return self._version

    @property
    def debug(self):
        """True if python library is built with Py_DEBUG"""
        # pylint: disable=attribute-defined-outside-init
        try:
            return self._debug
        except AttributeError:
            output = self._query(
                "import sys; print(int(hasattr(sys, 'gettotalrefcount')))"
            )
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
                    os.path.dirname(stdlib),
                    "libs",
                    "python{}{}.lib".format(version_nodot, postfix),
                )
            else:
                fname = self._get_config_var("LDLIBRARY")
                hints = []
                ld_library_path = os.getenv("LD_LIBRARY_PATH")
                if ld_library_path:
                    hints += ld_library_path.split(os.pathsep)
                hints += [
                    self._get_config_var("LIBDIR"),
                    self._get_config_var("LIBPL"),
                ]
                for dirname in hints:
                    candidate = os.path.join(dirname, fname)
                    if os.path.isfile(candidate):
                        self._library = candidate
                        break
                else:
                    raise RuntimeError("Unable to find {}".format(fname))
            return self._library

    def _get_python_path(self, key):
        return self._query(
            "import sysconfig; print(sysconfig.get_path({!r}))".format(key)
        )

    def _get_config_var(self, key):
        return self._query(
            "import sysconfig; print(sysconfig.get_config_var({!r}))".format(key)
        )

    def _query(self, script):
        return subprocess.check_output(
            [self.executable, "-c", script], universal_newlines=True
        ).strip()
