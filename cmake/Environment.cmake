# included by CMakeLists.txt to check the environment and build config headers

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckLibraryExists)
include(CheckFunctionExists)

macro(_try_compile VARIABLE fname msg ok fail)
	if (NOT DEFINED ${VARIABLE})
		message(STATUS "${msg}")
		try_compile(
			${VARIABLE}
			"${lass_BINARY_DIR}/cmake"
			"${lass_SOURCE_DIR}/cmake/${fname}"
		)
		if(${VARIABLE})
			message(STATUS "${msg} - ${ok}")
			set(${VARIABLE} 1 CACHE INTERNAL "${msg}")
		else()
			message(STATUS "${msg} - ${fail}")
			set(${VARIABLE} "" CACHE INTERNAL "${msg}")
		endif()
	endif()
endmacro()

macro(_try_compile_looking RESULT_VAR fname what)
	_try_compile(${RESULT_VAR} "${fname}" "Looking for ${what}" "found" "not found")
endmacro()

macro(_try_compile_checking RESULT_VAR fname what)
	_try_compile(${RESULT_VAR} "${fname}" "Checking whether ${what}" "yes" "no")
endmacro() 

# --- about Python ---

include(FindPythonInterp)
mark_as_advanced(CLEAR PYTHON_EXECUTABLE)

include(FindPythonLibs)
mark_as_advanced(CLEAR PYTHON_LIBRARY PYTHON_INCLUDE_DIR PYTHON_DEBUG_LIBRARY)
list(APPEND lass_INCLUDES ${PYTHON_INCLUDE_DIRS})

if(PYTHON_DEBUG_LIBRARY)
	set(LASS_PYTHON_HAS_DEBUG_BUILD 1)
else()
	set(LASS_PYTHON_HAS_DEBUG_BUILD 0)
	set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif()

if(NOT "${PYTHON_INCLUDE_DIR}" STREQUAL "${_TESTED_PYTHON_INCLUDE_PATH}")
	set(_python_header "${PYTHON_INCLUDE_DIR}/Python.h")
	message(STATUS "Looking in ${_python_header} for redefinitions")
	set(_TESTED_PYTHON_INCLUDE_PATH "${PYTHON_INCLUDE_DIR}" CACHE INTERNAL "" FORCE)
	unset(LASS_HAVE_PYTHON_POSIX_C_SOURCE CACHE)
	unset(LASS_HAVE_PYTHON_FILE_OFFSET_BITS CACHE)
	unset(LASS_HAVE_PYTHON_XOPEN_SOURCE CACHE)
	CHECK_SYMBOL_EXISTS("_POSIX_C_SOURCE" "${_python_header}" LASS_HAVE_PYTHON_POSIX_C_SOURCE)
	CHECK_SYMBOL_EXISTS("_FILE_OFFSET_BITS" "${_python_header}" LASS_HAVE_PYTHON_FILE_OFFSET_BITS)
	CHECK_SYMBOL_EXISTS("_XOPEN_SOURCE" "${_python_header}" LASS_HAVE_PYTHON_XOPEN_SOURCE)
	message(STATUS "Looking in ${_python_header} for redefinitions - done")
endif()


# --- check available headers ---

CHECK_INCLUDE_FILE("fcntl.h" LASS_HAVE_FCNTL_H)
CHECK_INCLUDE_FILE("limits.h" LASS_HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("poll.h" LASS_HAVE_POLL_H)
CHECK_INCLUDE_FILE("sched.h" LASS_HAVE_SCHED_H)
if(LASS_HAVE_SCHED_H)
	_try_compile_looking(LASS_HAVE_SCHED_H_CPU_SET_T "check_sched_h_cpu_set_t.cpp" "cpu_set_t")
endif()
CHECK_INCLUDE_FILE("stddef.h" HAVE_STDDEF_H)
set(LASS_HAVE_STDDEF_H ${HAVE_STDDEF_H})
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
	#set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
	set(CMAKE_REQUIRED_INCLUDES "pthread.h")
	set(CMAKE_REQUIRED_LIBRARIES ${threads_lib})
	CHECK_FUNCTION_EXISTS("pthread_setaffinity_np" LASS_HAVE_PTHREAD_H_PTHREAD_SETAFFINITY_NP)
endif()
#CHECK_INCLUDE_FILE("nptl/pthread.h" LASS_HAVE_NPTL_PTHREAD_H)


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
CHECK_SYMBOL_EXISTS("MultiByteToWideChar" "windows.h" LASS_HAVE_MULTIBYTETOWIDECHAR)



set(config_dir "${CMAKE_CURRENT_BINARY_DIR}/local")
set(config_file "${config_dir}/local_config.h")
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/lass/config/local_config.h.in" 
	"${config_file}")


