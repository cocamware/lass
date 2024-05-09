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


Requirements
------------

-   [Python](https://www.python.org/) >= 3.8
-   [CMake](https://cmake.org/) >= 3.16


Building
--------

Lass uses the CMake build system.

### Lass specific options

-   **`Lass_PYTHON_VERSION`** (`STRING`): If you want a specific version of
    Python, set this variable *before* running CMake for the first time
    in an empty build directory. It accepts both `<major>.<minor>` and exact
    `<major>.<minor>.<patch>` versions.
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

### Generic options

-   **`BUILD_SHARED_LIBS`** (`BOOL`): Build Lass as shared libraries instead
    of static libraries. `ON` by default.
-   **`BUILD_TESTING`** (`BOOL`): `ON` by default.


Tested platforms
----------------

Lass is build and tested on following combinations of platforms and tools.

Where possible, we rely on stock versions of the tools as provided by the
platform's package manager. Windows being the exception.
 
| OS               | Arch                       | CMake  | Python                                  | Compiler                               |
|------------------|----------------------------|--------|-----------------------------------------|----------------------------------------|
| Debian Bullseye  | x64, armv7l[1], aarch64[2] | 3.18.4 | 3.9.2                                   | gcc 10.2.1, clang 11.0.1               |
| Debian Bookworm  | x64                        | 3.25.1 | 3.11.2                                  | gcc 12.2.0, clang 14.0.6               |
| Ubuntu Focal     | x64                        | 3.16.3 | 3.8.2                                   | gcc 9.3.0, clang 10.0.0                |
| Ubuntu Yammy     | x64                        | 3.22.1 | 3.10.4                                  | gcc 11.2.0, clang 14.0.0               |
| Ubuntu Noble     | x64                        | 3.28.3 | 3.12.3                                  | gcc 13.2.0, clang 18.1.3               |
| Windows 10       | x86, x64                   | 3.26.5 | 3.8.10, 3.9.13, 3.10.11, 3.11.6, 3.12.0 | vs 2019 (v16.11.31), vs 2022 (v17.7.5) |

[1] armv7l support tested on Raspberry Pi 4 (ARM Cortex-A72, 32-bit Raspbian Bullseye,
CXXFLAGS=-mcpu=cortex-a72 -mfpu=neon-fp-armv8) and BeagleBoneBlack (ARM Cortex-A8,
Debian Bullseye, CXXFLAGS=-mcpu=cortex-a8 -mfpu=neon). On RPi4, the Clang build requires
C++17 or newer.

[2] aarch64 support tested on Raspberry Pi 4 (ARM Cortex-A72, 64-bit Raspbian Bullseye,
CXXFLAGS=-mcpu=cortex-a72). Just like x64, aarch64 will assume 48-bit pointer addresses
in TaggedPtr. volatile atomics are not supported anymore, use std::atomic.
