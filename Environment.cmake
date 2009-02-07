# included by CMakeLists.txt to check the environment and build config headers

include(FindPythonLibs)
mark_as_advanced(CLEAR PYTHON_LIBRARY PYTHON_INCLUDE_PATH PYTHON_DEBUG_LIBRARY)
list(APPEND lass_INCLUDES ${PYTHON_INCLUDE_PATH})

if(PYTHON_DEBUG_LIBRARY)
	set(LASS_PYTHON_HAS_DEBUG_BUILD 1)
else()
	set(LASS_PYTHON_HAS_DEBUG_BUILD 0)
	set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif()

include(FindPythonInterp)
mark_as_advanced(CLEAR PYTHON_EXECUTABLE)

include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckLibraryExists)
include(CheckFunctionExists)

CHECK_INCLUDE_FILE("fcntl.h" LASS_HAVE_FCNTL_H)
CHECK_INCLUDE_FILE("limits.h" LASS_HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("termios.h" LASS_HAVE_TERMIOS_H)
CHECK_INCLUDE_FILE("pthread.h" LASS_HAVE_PTHREAD_H)
CHECK_INCLUDE_FILE("sys/ioctl.h" LASS_HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILE("sys/mman.h" LASS_HAVE_SYS_MMAN_H)
CHECK_INCLUDE_FILE("sys/resource.h" LASS_HAVE_SYS_RESOURCE_H)
CHECK_INCLUDE_FILE("sys/socket.h" LASS_HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILE("sys/stat.h" LASS_HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("sys/filio.h" LASS_HAVE_SYS_FILIO_H)
CHECK_INCLUDE_FILE("sys/processor.h" LASS_HAVE_SYS_PROCESSOR_H)

CHECK_INCLUDE_FILE("unistd.h" LASS_HAVE_UNISTD_H)
if(LASS_HAVE_UNISTD_H)
	CHECK_SYMBOL_EXISTS("_SC_NPROCESSORS_CONF" "unistd.h" LASS_HAVE_UNISTD_H_SC_NPROCESSORS_CONF)
endif()

CHECK_INCLUDE_FILE("stdint.h" LASS_HAVE_STDINT_H)
if(LASS_HAVE_STDINT_H)
	try_compile(
		LASS_HAVE_STDINT_H_INT8_T_IS_CHAR
		"${lass_BINARY_DIR}/local"
		"${lass_SOURCE_DIR}/lass/config/check_int8_t_is_char.cpp"
		)
endif()

CHECK_INCLUDE_FILE("sched.h" LASS_HAVE_SCHED_H)
if(LASS_HAVE_STDINT_H)
	if ("LASS_HAVE_SCHED_H_CPU_SET_T" MATCHES "^LASS_HAVE_SCHED_H_CPU_SET_T$")
		message(STATUS "Looking for cpu_set_t")
		try_compile(
			LASS_HAVE_SCHED_H_CPU_SET_T
			"${lass_BINARY_DIR}/local"
			"${lass_SOURCE_DIR}/lass/config/check_sched_h_cpu_set_t.cpp"
			)
		if(LASS_HAVE_SCHED_H_CPU_SET_T)
			message(STATUS "Looking for cpu_set_t - found")
		else()
			message(STATUS "Looking for cpu_set_t - not found")
		endif()
	endif()
endif()

CHECK_LIBRARY_EXISTS("rt" "clock_gettime" "" LASS_HAVE_LIBRT)
if (LASS_HAVE_LIBRT)
   list(APPEND lass_LIBS "rt")
endif()

CHECK_LIBRARY_EXISTS("dl" "dlopen" "" LASS_HAVE_LIBDL)
if (LASS_HAVE_LIBDL)
   list(APPEND lass_LIBS "dl")
endif()

CHECK_LIBRARY_EXISTS("util" "openpty" "" LASS_HAVE_LIBUTIL)
if (LASS_HAVE_LIBUTIL)
   list(APPEND lass_LIBS "util")
endif()

CHECK_FUNCTION_EXISTS("strerror_r" LASS_HAVE_FUNC_STRERROR_R)
if(LASS_HAVE_FUNC_STRERROR_R)
	try_compile(
		LASS_HAVE_STRERROR_R_CHAR_P
		"${lass_BINARY_DIR}/local"
		"${lass_SOURCE_DIR}/lass/config/check_strerror_r_char_p.cpp"
		)
endif()

include(FindThreads)
set(LASS_HAVE_SPROC ${CMAKE_USE_SPROC_INIT})
set(LASS_HAVE_WIN32_THREADS ${CMAKE_USE_WIN32_THREADS_INIT})
set(LASS_HAVE_PTHREADS ${CMAKE_USE_PTHREADS_INIT})
set(LASS_HAVE_HP_PTHREADS ${CMAKE_HP_PTHREADS_INIT})
if (CMAKE_THREAD_LIBS_INIT)
	string(REGEX REPLACE "^-l" "" threads_lib ${CMAKE_THREAD_LIBS_INIT})
   	list(APPEND lass_LIBS ${threads_lib})
endif()

include(TestBigEndian)
TEST_BIG_ENDIAN(LASS_HAVE_BIG_ENDIAN)

if(NOT "${PYTHON_INCLUDE_PATH}" STREQUAL "${_TESTED_PYTHON_INCLUDE_PATH}")
	message(STATUS "Looking in Python.h for redefinitions ...")
	set(_TESTED_PYTHON_INCLUDE_PATH "${PYTHON_INCLUDE_PATH}" CACHE INTERNAL "" FORCE)
	set(LASS_HAVE_PYTHON_POSIX_C_SOURCE)
	set(LASS_HAVE_PYTHON_FILE_OFFSET_BITS)
	set(python_header "${PYTHON_INCLUDE_PATH}/Python.h")
	CHECK_SYMBOL_EXISTS("_POSIX_C_SOURCE" "${python_header}" LASS_HAVE_PYTHON_POSIX_C_SOURCE)
	CHECK_SYMBOL_EXISTS("_FILE_OFFSET_BITS" "${python_header}" LASS_HAVE_PYTHON_FILE_OFFSET_BITS)
endif()

set(config_dir ${lass_SOURCE_DIR}/lass/config)
set(config_file ${config_dir}/local_config.h)
configure_file(${config_dir}/local_config.h.in ${config_dir}/local_config.h)


