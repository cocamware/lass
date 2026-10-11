Lass - Library of Assembled Shared Sources
==========================================

Tom De Muer <tom@cocamware.com>  
Bram de Greve <bram@cocamware.com>

Lass is an open source C++ library written by Tom De Muer and Bram de Greve to
help the authors writing better code. It includes an interfacing library to 
Python, templated geometric primitives and spatial subdivision schemes, several
design patterns and other utilities the authors use in their day-to-day 
codings.


License
-------

Lass is dual licensed under CPAL-1.0 and GPL-2.0-or-later, see 
[COPYING](./COPYING) file.


Documentation
-------------

-   [Reference documentation (doxygen)](https://lass.cocamware.com/docs/doxygen/html/index.html)


Requirements
------------

-   C++17 or newer
-   [Python](https://www.python.org/) >= 3.10
-   [CMake](https://cmake.org/) >= 3.22
-   Optional: [Conan](https://conan.io/) >=2.12.0


Building with CMake
-------------------

Lass uses the CMake build system.

### Lass specific options

-   **`Lass_PYTHON_VERSION`** (`STRING`): If you want a specific version of
    Python, set this variable *before* running CMake for the first time
    in an empty build directory. It accepts both `<major>.<minor>` and exact
    `<major>.<minor>.<patch>` versions.
-   **`Lass_PYTHON_SABI`** (`STRING`): If set, build for Python's Stable ABI.
    Set this variable to the string `<major>.<minor>` of the minimum Python
    version you want the binaries to be compatible with. It should not be lower
    than `3.10` or higher than the Python version you're building with.
-   **`Python_EXECUTABLE`** (`FILEPATH`): If CMake cannot find the Python
    requested, you need to help it by specifying the full path to Python
    executable. Set this variable *before* running CMake for the first time
    in an empty build directory.
-   **`BUILD_SIMD_ALIGNED`** (`BOOL`): Align some data structures on 16-byte
    boundaries for better SSE2 alignment. But this does *not* necessarily
    improve performance! `OFF` by default.
-   **`BUILD_USING_PRECOMPILED_HEADERS`** (`BOOL`): Requires Microsoft Visual
    C++ compiler. `ON` by default, if this is the case.
-   **`BUILD_WITHOUT_ITERATOR_DEBUGGING`**: (`BOOL`): Requires Microsoft Visual
    C++ compiler. If set, it adds `/D_HAS_ITERATOR_DEBUGGING=0` as compiler
    option. It dramatically improves performance of a *Debug* build, but it
    is not ABI compatibily with binaries that don't use this setting. So, it is
    `OFF` by default.
-   **`Lass_HAVE_AVX`** (`BOOL`): Optimize some algorithms using the [AVX
    instruction set](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions)
    (not AVX2 or AVX-512). Requires a compatible x86 processor (Intel Sandy
    Bridge or newer, AMD Bulldozer or newer). `ON` by default for x86/x86-64
    architectures.
-   **`Lass_WITH_STUBGEN`** (`BOOL`): Generate Python *.pyi stubs and add
    `lass_stubgen` and `LassStubgen.cmake` to the installation.
    `ON` if Python version >= 3.10 and libclang(.dll|.so) can be found.
-   **`Lass_LIBCLANG_LIBRARY`**(`FILEPATH`): Override the full path to the
    libclang library to be used to generate the Python *.pyi stubs.
-   **`Lass_LIBCLANG_SYSTEM_INCLUDE_DIR`**(`PATH`): Override the Clang system
    include directory to be used to generate the Python *.pyi stubs. This typically
    is `"${Lass_LIBCLANG_LIBRARY}/../lib/clang/<version>/include"`.

### Generic options

-   **`BUILD_SHARED_LIBS`** (`BOOL`): Build Lass as shared libraries instead
    of static libraries. `ON` by default.
-   **`BUILD_TESTING`** (`BOOL`): `ON` by default.


Building with Conan
-------------------

Lass has a Conan 2 compatible recipe and can be used to build Lass locally, or
integrate it in your build system.

### Getting started

Lass finds Python through `syspython`, a small helper recipe that exists in this
repository. Export it once to your local Conan cache:

```sh
conan export tools/syspython
```

To build Lass locally and run the unit tests (add `-c tools.build:skip_test=True`
to skip them):

```sh
conan build .
```

To create a Lass package in your local Conan cache, and verify it with
`test_package`:

```sh
conan create .
```

Lass requires `compiler.cppstd=17` or newer: add it to your Conan profile or
pass it explicitly as `-s compiler.cppstd=17`.

By default, Python is found on your `PATH` (`python.exe` on Windows, `python3`
elsewhere). To use another Python, pass the full path to its executable:

```sh
conan build . -o "syspython/*:python_executable=/path/to/python"
```

The recipe derives its version from git tags, so build from a git clone with
tags, not from a source archive.

### Options

-   **`shared`** (`True`, `False`): Sets the `BUILD_SHARED_LIBS` CMake option.
    `True` by default.
-   **`fPIC`** (`True`, `False`): This option doesn't exist if `os` is
    `"Windows"`, and only has effect if `shared` is `False`. It sets the
    `CMAKE_POSITION_INDEPENDENT_CODE` CMake option. `True` by default.
-   **`simd_aligned`** (`True`, `False`): Sets the `BUILD_SIMD_ALIGNED` CMake
    option to align some structures on 16-byte boundaries. `False` by default.
-   **`without_iterator_debugging`** (`True`, `False`): sets the
    `BUILD_WITHOUT_ITERATOR_DEBUGGING` CMake option. `False` by default.
-   **`have_avx`** (`True`, `False`): This option only exists if `arch` is
    `"x86"` or `"x86_64"`. It sets the `Lass_HAVE_AVX` CMake option to enable
    the AVX instruction set. `True` by default.
-   **`with_stubgen`** (`True`, `False`, `"auto"`): Sets the `Lass_WITH_STUBGEN`
    CMake option to generate Python *.pyi stubs. If `"auto"`, then this will
    depend on the Python version and availability of libclang. `"auto"` by
    default.
-   **`py_stable_abi`** (`False`, `"<major>.<minor>"`): if not `False`, set
    the `Lass_PYTHON_SABI` CMake option to build for the Stable ABI. `False` by
    default.


Python Stable ABI (Limited C API)
---------------------------------

Lass can be built to only use [Python's Stable ABI](https://docs.python.org/3/c-api/stable.html)
to be compatible with multiple Python versions at once. To accomplish this, Lass
will be restricted to use Python's Limited C API. Code outside the Limited C API
is replaced by alternative calls that may be slower. Normal builds use the full
Python C API to not sacrifice performance.

To enable this, you must specify the minimum Python version you want the
binaries to be compatible with, either using the CMake option `Lass_PYTHON_SABI`
or the Conan option `py_stable_abi`. This version is specified as a string of
the format `<major>.<minor>` and cannot be lower than the oldest Python version
supported by Lass (currently Python 3.10), or higher than the Python version
you're using to build Lass.

**NOTE**: This requires CMake >= 3.26

**NOTE**: The Stable ABI is not supported on free-threaded Python builds (and
Lass itself does not support free-threaded Python yet anyway).

**NOTE**: By default, Lass will _not_ be built for the Stable ABI, but uses the
full width of the Python API.

**NOTE**: Building Lass against the Stable ABI does _not_ imply that the
extension modules built with Lass will also target the Stable ABI. You must
explicitly pass the same Stable ABI version when defining the module in CMake:

```cmake
Python_add_library(mymodule WITH_SOABI USE_SABI "${Lass_PYTHON_SABI}")
target_link_libraries(mymodule PRIVATE Lass::lass_python)
```

Lass provides a shorthand function for this. It sets `USE_SABI` to the same
value as Lass if Lass was built for the Stable ABI, and also links to
`Lass::lass_python`:

```cmake
Lass_Python_add_library(mymodule WITH_SOABI)
```

Lass also provides a function to define the `Py_LIMITED_API` macro on a target
that already links to `Lass::lass_python` (or `Python::SABIModule`). It does
nothing if Lass was not built for the Stable ABI:

```cmake
Lass_target_limited_api(mytarget)
```


Tested platforms
----------------

Lass is build and tested on following combinations of platforms and tools.

Where possible, we rely on stock versions of the tools as provided by the
platform's package manager. Windows being the exception.
 
| OS               | Arch                       | CMake  | Python                                              | Compiler                               |
|------------------|----------------------------|--------|-----------------------------------------------------|----------------------------------------|
| Debian Bookworm  | x64, armv7l[1], aarch64[2] | 3.25.1 | 3.11.2                                              | gcc 12.2.0, clang 14.0.6               |
| Debian Trixie    | x64                        | 3.31.6 | 3.13.5                                              | gcc 14.2.0, clang 19.1.7               |
| Ubuntu Jammy     | x64                        | 3.22.1 | 3.10.12                                             | gcc 11.4.0, clang 14.0.0               |
| Ubuntu Noble     | x64                        | 3.28.3 | 3.12.3                                              | gcc 13.3.0, clang 18.1.3               |
| Ubuntu Resolute  | x64                        | 4.2.3  | 3.14.4                                              | gcc 15.2.0, clang 21.1.8               |
| Windows LTSC2025 | x86, x64                   | 4.2.2  | 3.10.11, 3.11.9, 3.12.10, 3.13.10, 3.14.2, 3.15.0b2 | vs 2022 (v17.14.35), vs 2026 (v18.7.1) |

[1] armv7l support tested on BeagleBoneBlack (ARM Cortex-A8, Debian Bookworm,
CXXFLAGS=-mcpu=cortex-a8 -mfpu=neon).

[2] aarch64 support tested on Raspberry Pi 4 (ARM Cortex-A72, 64-bit Debian Bookworm,
CXXFLAGS=-mcpu=cortex-a72). Just like x64, aarch64 will assume 48-bit pointer addresses
in TaggedPtr. volatile atomics are not supported anymore, use std::atomic.
