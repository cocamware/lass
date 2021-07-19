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

-   [Python](https://www.python.org/) >= 2.7
-   [CMake](https://cmake.org/) >= 3.0


Building
--------

Lass uses the CMake build system.

### Lass specific options

-   **`Lass_PYTHON_VERSION`** (`STRING`): If you want a specific version of
    Python, set this variable *before* running CMake for the first time
    in an empty build directory. It accepts both `<major>.<minor>` and exact
    `<major>.<minor>.<patch>` versions (except CMake 3.0 which only accepts the
    exact version).
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
-   **`LASS_HAVE_STD_AUTO_PTR`** (`BOOL`): In Lass 1.9, the usage of 
    `std::auto_ptr` is replaced by `std::unique_ptr`. To force compatibility
    with earlier versions, you can set this flag to keep using `std::auto_ptr`
    in the public API. `OFF` by default, except if the compiler does not yet
    support `std::unique_ptr`.

### Generic options

-   **`BUILD_SHARED_LIBS`** (`BOOL`): Build Lass as shared libraries instead
    of static libraries. `ON` by default.
-   **`BUILD_TESTING`** (`BOOL`): `ON` by default.


Tested platforms
----------------

Lass is build and tested on following combinations of platforms and tools.

Where possible, we rely on stock versions of the tools as provided by the
platform's package manager. Windows being the exception.

| OS               | Arch     | CMake  | Python                     | Compiler                  |
|------------------|----------|--------|----------------------------|---------------------------|
| Debian Jessie    | x64      | 3.0.2  | 2.7.9,  3.4.2              | gcc 4.9.2, clang 3.5.0    |
| Debian Stretch   | x64      | 3.7.2  | 2.7.13, 3.5.3              | gcc 6.3.0, clang 3.8.1    |
| Debian Buster    | x64      | 3.12.1 | 2.7.15, 3.6.6              | gcc 8.2.0, clang 6.0.1    |
| Ubuntu Xenial    | x64      | 3.5.1  | 2.7.12, 3.5.2              | gcc 5.4.0, clang 3.8.0    |
| Ubuntu Bionic    | x64      | 3.10.2 | 2.7.15, 3.6.5              | gcc 7.3.0, clang 6.0.0    |
| Ubuntu Focal     | x64      | 3.16.3 | 2.7.18, 3.8.2              | gcc 9.3.0, clang 10.0.0   |
| Ubuntu Hirsute   | x64      | 3.18.4 | 2.7.18, 3.9.5              | gcc 10.3.0, clang 12.0.0  |
| Windows 10       | x86, x64 | 3.18.4 | 2.7.18                     | vs 2008                   |
| Windows 10       | x86, x64 | 3.18.4 | 3.6.8, 3.7.9, 3.8.6, 3.9.0 | vs 2017, vs 2019          |
