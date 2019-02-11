# included by CMakeLists.txt to check the environment and build config headers

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckLibraryExists)
include(CheckFunctionExists)

function(_try_compile VARIABLE fname msg ok fail)
	if (NOT DEFINED ${VARIABLE})
		message(STATUS "${msg}")
		try_compile(
			${VARIABLE}
			"${Lass_BINARY_DIR}/cmake"
			"${Lass_SOURCE_DIR}/cmake/${fname}"
		)
		if(${VARIABLE})
			message(STATUS "${msg} - ${ok}")
			set(${VARIABLE} 1 CACHE INTERNAL "${msg}")
		else()
			message(STATUS "${msg} - ${fail}")
			set(${VARIABLE} "" CACHE INTERNAL "${msg}")
		endif()
	endif()
endfunction()

function(_try_compile_looking RESULT_VAR fname what)
	_try_compile(${RESULT_VAR} "${fname}" "Looking for ${what}" "found" "not found")
endfunction()

function(_try_compile_checking RESULT_VAR fname what)
	_try_compile(${RESULT_VAR} "${fname}" "Checking whether ${what}" "yes" "no")
endfunction() 

# --- about Python ---

unset(_Lass_FIND_PYTHON_ARGS)
if(Lass_PYTHON_VERSION)
    set(_Lass_FIND_PYTHON_ARGS "${Lass_PYTHON_VERSION}" EXACT)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(PythonCompat ${_Lass_FIND_PYTHON_ARGS} REQUIRED
             COMPONENTS Interpreter OPTIONAL_COMPONENTS Development)
mark_as_advanced(CLEAR Python_EXECUTABLE)

if(Python_Development_FOUND)
	mark_as_advanced(CLEAR Python_LIBRARY_RELEASE Python_INCLUDE_DIR Python_LIBRARY_DEBUG)

	if(Python_LIBRARY_DEBUG)
		set(LASS_PYTHON_HAS_DEBUG_BUILD 1)
	else()
		set(LASS_PYTHON_HAS_DEBUG_BUILD 0)
	endif()

	if(NOT "${Python_INCLUDE_DIR}" STREQUAL "${_Lass_Python_INCLUDE_DIR}")
		set(_python_header "${Python_INCLUDE_DIR}/Python.h")
		message(STATUS "Looking in ${_python_header} for redefinitions")
		set(_Lass_Python_INCLUDE_DIR "${Python_INCLUDE_DIR}" CACHE INTERNAL "" FORCE)
		unset(LASS_HAVE_PYTHON_POSIX_C_SOURCE CACHE)
		unset(LASS_HAVE_PYTHON_FILE_OFFSET_BITS CACHE)
		unset(LASS_HAVE_PYTHON_XOPEN_SOURCE CACHE)
		CHECK_SYMBOL_EXISTS("_POSIX_C_SOURCE" "${_python_header}" LASS_HAVE_PYTHON_POSIX_C_SOURCE)
		CHECK_SYMBOL_EXISTS("_FILE_OFFSET_BITS" "${_python_header}" LASS_HAVE_PYTHON_FILE_OFFSET_BITS)
		CHECK_SYMBOL_EXISTS("_XOPEN_SOURCE" "${_python_header}" LASS_HAVE_PYTHON_XOPEN_SOURCE)
		message(STATUS "Looking in ${_python_header} for redefinitions - done")
	endif()
endif()

# --- check available headers ---

CHECK_INCLUDE_FILE("fcntl.h" LASS_HAVE_FCNTL_H)
CHECK_INCLUDE_FILE("limits.h" LASS_HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("poll.h" LASS_HAVE_POLL_H)
CHECK_INCLUDE_FILE("sched.h" LASS_HAVE_SCHED_H)
if(LASS_HAVE_SCHED_H)
	_try_compile_looking(LASS_HAVE_SCHED_H_CPU_SET_T "check_sched_h_cpu_set_t.cpp" "cpu_set_t")
endif()
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
set(LASS_HAVE_STDINT_H ${HAVE_STDINT_H})
if(LASS_HAVE_STDINT_H)
	_try_compile_checking(LASS_HAVE_STDINT_H_INT8_T_IS_CHAR "check_int8_t_is_char.cpp" "int8_t is char")
endif()
CHECK_INCLUDE_FILE("termios.h" LASS_HAVE_TERMIOS_H)
CHECK_INCLUDE_FILE("unistd.h" LASS_HAVE_UNISTD_H)
if(LASS_HAVE_UNISTD_H)
	CHECK_SYMBOL_EXISTS("_SC_NPROCESSORS_CONF" "unistd.h" LASS_HAVE_UNISTD_H_SC_NPROCESSORS_CONF)
endif()
CHECK_INCLUDE_FILES("sys/param.h;sys/cpuset.h" LASS_HAVE_SYS_CPUSET_H) # cpuset.h also needs param.h (at least on FreeBSD)
CHECK_INCLUDE_FILE("sys/filio.h" LASS_HAVE_SYS_FILIO_H)
CHECK_INCLUDE_FILE("sys/ioctl.h" LASS_HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILE("sys/mman.h" LASS_HAVE_SYS_MMAN_H)
CHECK_INCLUDE_FILE("sys/processor.h" LASS_HAVE_SYS_PROCESSOR_H)
CHECK_INCLUDE_FILE("sys/resource.h" LASS_HAVE_SYS_RESOURCE_H)
CHECK_INCLUDE_FILE("sys/socket.h" LASS_HAVE_SYS_SOCKET_H)
if (LASS_HAVE_SYS_SOCKET_H)
	# sometimes linux/un.h needs sys/socket.h to be included first, so use a special crafted test
	_try_compile_looking(LASS_HAVE_LINUX_UN_H "check_linux_un_h.cpp" "linux/un.h")
endif()
CHECK_INCLUDE_FILE("sys/syscall.h" LASS_HAVE_SYS_SYSCALL_H)
if(LASS_HAVE_SYS_SYSCALL_H)
	CHECK_SYMBOL_EXISTS("__NR_gettid" "sys/syscall.h" LASS_HAVE_SYS_SYSCALL_H_GETTID)
endif()
CHECK_INCLUDE_FILE("sys/sysctl.h" LASS_HAVE_SYS_SYSCTL_H)
CHECK_INCLUDE_FILE("sys/stat.h" LASS_HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("sys/time.h" LASS_HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("sys/un.h" LASS_HAVE_SYS_UN_H)
set(LASS_HAVE_SYS_TYPES_H ${HAVE_SYS_TYPES_H})
CHECK_INCLUDE_FILE("mach/thread_policy.h" LASS_HAVE_MACH_THREAD_POLICY_H)


# --- Threading ----

# this one is rather complicated =)

include(FindThreads)
set(LASS_HAVE_PTHREAD_H ${CMAKE_HAVE_PTHREAD_H})
set(LASS_HAVE_SPROC ${CMAKE_USE_SPROC_INIT})
set(LASS_HAVE_WIN32_THREADS ${CMAKE_USE_WIN32_THREADS_INIT})
set(LASS_HAVE_PTHREADS ${CMAKE_USE_PTHREADS_INIT})
set(LASS_HAVE_HP_PTHREADS ${CMAKE_HP_PTHREADS_INIT})
if (CMAKE_THREAD_LIBS_INIT)
	string(REGEX REPLACE "^-l" "" threads_lib ${CMAKE_THREAD_LIBS_INIT})
	list(APPEND lass_LIBS ${threads_lib})
endif()
if(LASS_HAVE_PTHREAD_H)
	CHECK_INCLUDE_FILE("pthread_np.h" LASS_HAVE_PTHREAD_NP_H)
	set(CMAKE_REQUIRED_INCLUDES "pthread.h")
	set(CMAKE_REQUIRED_LIBRARIES ${threads_lib})
	CHECK_FUNCTION_EXISTS("pthread_setaffinity_np" LASS_HAVE_PTHREAD_SETAFFINITY_NP)
endif()


# --- ARM ---

function(_get_cpuinfo_field key cpuinfo VALUE)
    if (cpuinfo MATCHES "${key}[ \t\r\n]*:[ \t\r\n]*([^\n]*)")
        set(${VALUE} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    else()
        set(${VALUE} PARENT_SCOPE)
    endif()
endfunction()

_try_compile_checking(LASS_HAVE_ARM "check_arm.cpp" "targeting ARM")
if (LASS_HAVE_ARM)
    # INCOMPLETE DETECTION OF HARDWARE ...

    set(_mcpu)
    if (CMAKE_CXX_FLAGS MATCHES "(-mcpu=[^ \t\n]*)")
        set(_mcpu "${CMAKE_MATCH_1}")
    else()
        set(msg "Determining CPU")
        message(STATUS "${msg}")
        file(READ "/proc/cpuinfo" _cpuinfo)
        _get_cpuinfo_field("CPU implementer" "${_cpuinfo}" _cpu_implementer)
        _get_cpuinfo_field("CPU part" "${_cpuinfo}" _cpu_part)
        _get_cpuinfo_field("Hardware" "${_cpuinfo}" _hardware)
        if (_hardware STREQUAL "BCM2708") # Raspberry Pi
            set(_mcpu "-mcpu=arm1176jzf-s")
        elseif (_cpu_implementer STREQUAL "0x41") # ARM
            if (_cpu_part STREQUAL "0xc08")
                set(_mcpu "-mcpu=cortex-a8")
            elseif (_cpu_part STREQUAL "0xc09")
                set(_mcpu "-mcpu=cortex-a9")
            endif()
        endif()
        if (_mcpu)
            message(STATUS "${msg} - ${_mcpu}")
            lass_set_cache_value(CMAKE_CXX_FLAGS " ${_mcpu}" APPEND_STRING)
            mark_as_advanced(CLEAR CMAKE_CXX_FLAGS)
        else()
            message(STATUS "${msg} - unknown")
        endif()
    endif()
    _lass_mark_key_for_export(CMAKE_CXX_FLAGS "-mcpu")
    
    set(_march)
    if (CMAKE_CXX_FLAGS MATCHES "(-march=[^ \t\n]*)")
        set(_march "${CMAKE_MATCH_1}")
    elseif(NOT _mcpu)
        set(msg "Determining ARCH")
        message(STATUS "${msg}")
        file(READ "/proc/cpuinfo" _cpuinfo)
        _get_cpuinfo_field("CPU architecture" "${_cpuinfo}" _cpu_architecture)
        if (_cpu_architecture STREQUAL "5TE")
            set(_march "-march=armv5te")
        elseif (_cpu_architecture STREQUAL "7")
            set(_march "-march=armv7")
        endif()
        if (_march)
            message(STATUS "${msg} - ${_march}")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_march}" CACHE STRING "" FORCE)
            mark_as_advanced(CLEAR CMAKE_CXX_FLAGS)
        else()
            message(STATUS "${msg} - unknown")
        endif()
    endif()
    _lass_mark_key_for_export(CMAKE_CXX_FLAGS "-march")
    
    set(_mfpu)
    if (CMAKE_CXX_FLAGS MATCHES "(-mfpu=[^ \t\n]*)")
        set(_mfpu "${CMAKE_MATCH_1}")
    else()
        set(msg "Determining FPU")
        message(STATUS "${msg}")
        file(READ "/proc/cpuinfo" _cpuinfo)
        _get_cpuinfo_field("Features" "${_cpuinfo}" _features)
        # making sure all features are nicely surrounded by ;
        string(REGEX REPLACE "[ \t\n]+" ";" _features "${_features}")
        set(_features ";${_features};")
        if (_features MATCHES ";neon;")
            set(_mfpu "-mfpu=neon")
        elseif (_features MATCHES ";vfpv3;")
            set(_mfpu "-mfpu=vfpv3")
        elseif (_features MATCHES ";vfp;")
            set(_mfpu "-mfpu=vfp")
        else()
            set(_mfpu)
        endif()
        if (_mfpu)
            message(STATUS "${msg} - ${_mfpu}")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_mfpu}" CACHE STRING "" FORCE)
            mark_as_advanced(CLEAR CMAKE_CXX_FLAGS)
        else()
            message(STATUS "${msg} - unknown")
        endif()
    endif()
    _lass_mark_key_for_export(CMAKE_CXX_FLAGS "-mfpu")
    _lass_mark_key_for_export(CMAKE_CXX_FLAGS "-mfloat-abi")
        
    # Checking support of some assembly instructions
    _try_compile_checking(LASS_HAVE_LDREXB_STREXB "check_ldrexb_strexb.cpp" "ldrexb and strexb are supported")
    _try_compile_checking(LASS_HAVE_LDREXH_STREXH "check_ldrexh_strexh.cpp" "ldrexh and strexh are supported")
    _try_compile_checking(LASS_HAVE_LDREX_STREX "check_ldrex_strex.cpp" "ldrex and strex are supported")
    _try_compile_checking(LASS_HAVE_LDREXD_STREXD "check_ldrexd_strexd.cpp" "ldrexd and strexd are supported")
    
    set(VARIABLE LASS_KUSER_HELPER_VERSION)
    set(msg "Looking for __kuser_helper_version")
    if (NOT DEFINED ${VARIABLE})
        message(STATUS "${msg}")
        try_run(
            ${VARIABLE}_run ${VARIABLE}_compile
            "${Lass_BINARY_DIR}/cmake"
            "${Lass_SOURCE_DIR}/cmake/check_kuser_helper_version.cpp"
        )
        if (${VARIABLE}_compile AND NOT ${VARIABLE}_run STREQUAL "FAILED_TO_RUN")
            message(STATUS "${msg} - ${${VARIABLE}_run}")
            set(${VARIABLE} ${${VARIABLE}_run} CACHE INTERNAL "${msg}")
        else()
            message(STATUS "${msg} - not found")
            set(${VARIABLE} "" CACHE INTERNAL "${msg}")
        endif()
    endif()
endif()
    
# --- extra libraries ---

if (WIN32)
	list(APPEND lass_LIBS "ole32") # for Crashdump
endif()

CHECK_LIBRARY_EXISTS("rt" "clock_gettime" "" LASS_HAVE_CLOCK_GETTIME)
if (LASS_HAVE_CLOCK_GETTIME)
   list(APPEND lass_LIBS "rt")
endif()

CHECK_LIBRARY_EXISTS("dl" "dlopen" "" LASS_HAVE_DLOPEN)
if (LASS_HAVE_DLOPEN)
   list(APPEND lass_LIBS "dl")
endif()

set(LASS_HAVE_EXPM1)
CHECK_SYMBOL_EXISTS("expm1" "math.h" LASS_HAVE_MATH_H_EXPM1)
if (NOT LASS_HAVE_MATH_H_EXPM1)
	CHECK_LIBRARY_EXISTS("m" "expm1" "" LASS_HAVE_LIBM_EXPM1)
	if (LASS_HAVE_LIBM_EXPM1)
		list(APPEND lass_LIBS "m")
	endif()
endif()
if (LASS_HAVE_MATH_H_EXPM1 OR LASS_HAVE_LIBM_EXPM1)
	set(LASS_HAVE_EXPM1 1)
endif()

# --- check availability of some general functions ---

CHECK_FUNCTION_EXISTS("strerror_r" LASS_HAVE_FUNC_STRERROR_R)
if(LASS_HAVE_FUNC_STRERROR_R)
	_try_compile_checking(LASS_HAVE_STRERROR_R_CHAR_P "check_strerror_r_char_p.cpp" "strerror_r returns char*")
endif()
CHECK_FUNCTION_EXISTS("aligned_alloc" LASS_HAVE_ALIGNED_ALLOC)

# If compiler supports newer std::unique_ptr, disable usage of std::auto_ptr, 
# to avoid deprecation warnings. But provide option to user to override in 
# any case.
_try_compile_looking(LASS_HAVE_STD_UNIQUE_PTR "check_std_unique_ptr.cpp" "std::unique_ptr")
if(LASS_HAVE_STD_UNIQUE_PTR)
    set(_lass_have_std_auto_ptr_default OFF)
else()
    set(_lass_have_std_auto_ptr_default ON)
endif()
option(LASS_HAVE_STD_AUTO_PTR "Using std::auto_ptr in Lass API. Disabled by default if std::unique_ptr is found." "${_lass_have_std_auto_ptr_default}")


# --- checking some properties of numbers and available functions ---

include(TestBigEndian)
TEST_BIG_ENDIAN(LASS_HAVE_BIG_ENDIAN)

include(CheckTypeSize)
CHECK_TYPE_SIZE("long long" LONG_LONG)
set(LASS_HAVE_LONG_LONG ${HAVE_LONG_LONG})

CHECK_SYMBOL_EXISTS("llabs" "stdlib.h" LASS_HAVE_LLABS)
CHECK_SYMBOL_EXISTS("_abs64" "stdlib.h" LASS_HAVE_ABS64)
_try_compile_looking(LASS_HAVE_LONG_DOUBLE_STD_FUNCTIONS "check_long_double_std_functions.cpp" "fabsl")


# --- wide character support ---

CHECK_TYPE_SIZE(wchar_t WCHAR_T)
set(LASS_HAVE_WCHAR_T ${HAVE_WCHAR_T})
CHECK_SYMBOL_EXISTS("_wfopen" "stdio.h" LASS_HAVE_WFOPEN)
CHECK_FUNCTION_EXISTS("iconv" LASS_HAVE_ICONV)
if(LASS_HAVE_ICONV)
	_try_compile_checking(LASS_HAVE_ICONV_CONST_CHAR "check_iconv_const_char.cpp" "iconv inbuf parameter is const char**")
endif()
CHECK_SYMBOL_EXISTS("MultiByteToWideChar" "windows.h" LASS_HAVE_MULTIBYTETOWIDECHAR)


configure_file(
	"${Lass_SOURCE_DIR}/lass/config/local_config.h.in" 
	"${Lass_BINARY_DIR}/local/local_config.h"
    )


