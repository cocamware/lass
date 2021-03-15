"""
setuptools setup script for pylass using CMake

This setup script will build pylass using CMake and install it in
site-packages:

python setup.py install

Requires CMake and a proper environment for the compiler.
"""
import os
import re
import subprocess
import sys
from distutils import ccompiler, log, sysconfig, util
from distutils.command.build import build as _build

from setuptools import Command, setup
from setuptools.command.install import install as _install


class build(_build):
    """
    patched build command to add build_cmake as sub command
    """

    sub_commands = _build.sub_commands + [
        ("build_cmake", lambda _: True),
    ]

    def initialize_options(self):
        _build.initialize_options(self)

    def finalize_options(self):
        _build.finalize_options(self)
        self.build_lib = self.build_platlib  # force it to use build_platlib


class install(_install):
    """
    patched install command to allways run install_lib
    """

    sub_commands = [
        ("install_lib", lambda _: True) if cmd == "install_lib" else (cmd, pred)
        for cmd, pred in _install.sub_commands
    ]


class build_cmake(Command):
    """A custom command to run a CMake build."""

    description = "run cmake build"
    user_options = [
        # The format is (long option, short option, description).
        ("build-lib=", "b", "directory for compiled extension modules"),
        ("build-temp=", "t", "directory for temporary files (build by-products)"),
        (
            "plat-name=",
            "p",
            "platform name to cross-compile for, if supported "
            "(default: %s)" % util.get_platform(),
        ),
        ("debug", "g", "compile with debugging information"),
        ("force", "f", "forcibly build everything (ignore file timestamps)"),
        ("compiler=", "c", "specify the compiler type"),
    ]

    boolean_options = ["debug", "force"]

    help_options = [
        ("help-compiler", None, "list available compilers", ccompiler.show_compilers),
    ]

    def initialize_options(self):
        """Set default values for options."""
        # Each user option must be listed here with their default value.
        self.build_lib = None
        self.build_temp = None
        self.plat_name = None
        self.debug = None
        self.force = 0
        self.compiler = None

    def finalize_options(self):
        """Post-process options."""
        self.set_undefined_options(
            "build",
            ("build_lib", "build_lib"),
            ("build_temp", "build_temp"),
            ("plat_name", "plat_name"),
            ("debug", "debug"),
            ("force", "force"),
            ("compiler", "compiler"),
        )
        if not self.compiler:
            self.compiler = ccompiler.get_default_compiler()

    def run(self):
        env = self._get_env()
        self._configure(env)
        self._build(env)
        self._copy()

    def _configure(self, env):
        generator = "NMake Makefiles" if sys.platform == "win32" else "Unix Makefiles"
        if hasattr(sys, "gettotalrefcount"):
            cmake_build_type = "Debug"  # https://stackoverflow.com/a/647312
        else:
            cmake_build_type = "RelWithDebInfo" if self.debug else "Release"
        print("cmake_build_type: {}".format(cmake_build_type))
        python_version = ".".join(map(str, sys.version_info[:2]))
        python_executable = sys.executable
        python_library = get_python_library()
        python_include_dir = sysconfig.get_python_inc()

        command = [
            "cmake",
            "-G",
            generator,
            "-DBUILD_SHARED_LIBS=OFF",
            "-DBUILD_TESTING=OFF",
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON",
            "-DCMAKE_BUILD_TYPE={}".format(cmake_build_type),
            "-DLass_PYTHON_VERSION={}".format(python_version),
            "-DPython_EXECUTABLE={}".format(fwdslash(python_executable)),
            "-DPython_LIBRARY_RELEASE={}".format(fwdslash(python_library)),
            "-DPython_LIBRARY_DEBUG={}".format(fwdslash(python_library)),
            "-DPython_INCLUDE_DIR={}".format(fwdslash(python_include_dir)),
            os.getcwd(),
        ]

        self.mkpath(self.build_temp)
        self._check_call(command, env=env, cwd=self.build_temp)

    def _build(self, env):
        command = ["cmake", "--build", os.curdir, "--target", "pylass"]

        if self._cmake_version and self._cmake_version >= (3, 12):
            command += ["--parallel"]

        self._check_call(command, cwd=self.build_temp, env=env)

    def _copy(self):
        self.copy_tree(os.path.join(self.build_temp, "bin"), self.build_lib)

    def _check_call(self, command, **kwargs):
        self.announce("Running command: {}".format(command), level=log.INFO)
        if not self.dry_run:
            subprocess.check_call(command, **kwargs)

    def _get_env(self):
        compiler = ccompiler.new_compiler(
            compiler=self.compiler, dry_run=self.dry_run, force=self.force
        )
        try:
            compiler.initialize()  # MSVCCompiler
        except AttributeError:
            pass
        sysconfig.customize_compiler(compiler)

        env = os.environ.copy()

        try:
            cc = compiler.compiler[0]
        except AttributeError:
            cc = compiler.cc  # MSVCCompiler
        print(cc)
        env.setdefault("CC", cc)

        try:
            cxx = compiler.compiler_cxx[0]
        except AttributeError:
            cxx = compiler.cc  # MSVCCompiler
        print(cxx)
        env.setdefault("CXX", cxx)

        try:
            env["PATH"] = compiler._paths
        except AttributeError:
            pass

        if compiler.include_dirs:
            env["INCLUDE"] = os.pathsep.join(compiler.include_dirs)

        if compiler.library_dirs:
            env["LIB"] = os.pathsep.join(compiler.library_dirs)

        return env

    @property
    def _cmake_version(self):
        try:
            return self.__cmake_version
        except AttributeError:
            out = subprocess.check_output(
                ["cmake", "--version"], universal_newlines=True
            )
            match = re.match(r"^cmake version (\d+)\.(\d+)", out)
            if match:
                self.__cmake_version = int(match.group(1)), int(match.group(2))
            else:
                self.__cmake_version = None
            return self.__cmake_version


def fwdslash(path):
    "Convert backslashes to forward slashes"
    return path.replace("\\", "/")


def get_python_library():
    "return path to python link library (e.g. pythonXY.lib on Windows)"
    prefix = get_exec_prefix()

    if sys.platform == "win32":
        template = "python{}{}"
        # if self.debug:
        #    template = template + '_d'
        template += ".lib"
        pythonlib = template.format(sys.hexversion >> 24, (sys.hexversion >> 16) & 0xFF)
        return os.path.join(prefix, "libs", pythonlib)

    libpl = sysconfig.get_config_var("LIBPL")
    ldlibrary = sysconfig.get_config_var("LDLIBRARY")
    return os.path.join(libpl, ldlibrary)


def get_exec_prefix():
    """
    Return the prefix that should house the link library.

    Virtual environments usually don't include it, so you need to go back
    to the original installation.
    """
    # venv:
    try:
        return sys.base_exec_prefix
    except AttributeError:
        pass

    # virtualenv:
    try:
        return sys.real_prefix  # virtualenv telling original location
    except AttributeError:
        pass

    return sys.exec_prefix


def get_version():
    self_dir = os.path.dirname(os.path.abspath(__file__))
    with open(os.path.join(self_dir, "CMakeLists.txt")) as fp:
        content = fp.read()
    match = re.search(r"project\(Lass VERSION (\d+\.\d+\.\d+)\)", content)
    assert match
    version = match.group(1)

    version_tag = "lass-{}".format(version)
    try:
        describe = subprocess.check_output(
            ["git", "describe", "--dirty=.dirty", "--tags", "--match", version_tag],
            cwd=self_dir,
            universal_newlines=True,
        ).strip()
    except subprocess.CalledProcessError:
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
    base_tag = "lass-1.10.1"

    describe = subprocess.check_output(
        ["git", "describe", "--dirty=.dirty", "--tags", "--match", base_tag],
        cwd=self_dir,
        universal_newlines=True,
    ).strip()
    assert describe.startswith(base_tag), "{!r} does not start with {!r}".format(
        describe, base_tag
    )
    match = re.search(
        r"{}-(\d+)-(g\w+(\.dirty)?)$".format(re.escape(base_tag)), describe
    )
    assert match, "unexpected describe format: {!r}".format(describe)
    pre_release, rev = match.group(1), match.group(2)
    return "{}.dev{}+{}".format(version, pre_release, rev)


setup(
    name="pylass",
    version=get_version(),
    author="Cocamware",
    author_email="info@cocamware.com",
    description="Python bindings to Lass",
    url="https://lass.cocamware.com/",
    cmdclass={"build": build, "build_cmake": build_cmake, "install": install,},
    zip_safe=False,
)
