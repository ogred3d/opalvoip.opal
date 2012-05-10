/* config_in.h.  Generated from configure.in by autoheader.  */

/* Define if building for a CISC machine (e.g. Intel). */
#define CPU_CISC 1

/* Define if building for a RISC machine (assume slow byte access). */
#undef CPU_RISC

/* Path to random device */
#undef DEV_URANDOM

/* Define to compile in dynamic debugging system. */
#undef ENABLE_DEBUGGING

/* Report errors to this file. */
#undef ERR_REPORTING_FILE

/* Define to use logging to stdout. */
#undef ERR_REPORTING_STDOUT

/* Define this to use ISMAcryp code. */
#undef GENERIC_AESICM

/* Define to 1 if you have the <arpa/inet.h> header file. */
#undef HAVE_ARPA_INET_H

/* Define to 1 if you have the <byteswap.h> header file. */
#undef HAVE_BYTESWAP_H

/* Define to 1 if you have the `inet_aton' function. */
#undef HAVE_INET_ATON

/* Define to 1 if the system has the type `int16_t'. */
#define HAVE_INT16_T 1

/* Define to 1 if the system has the type `int32_t'. */
#define HAVE_INT32_T 1

/* Define to 1 if the system has the type `int8_t'. */
#define HAVE_INT8_T 1

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the `socket' library (-lsocket). */
#undef HAVE_LIBSOCKET

/* Define to 1 if you have the <machine/types.h> header file. */
#undef HAVE_MACHINE_TYPES_H

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#undef HAVE_NETINET_IN_H

/* Define to 1 if you have the `socket' function. */
#undef HAVE_SOCKET

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <syslog.h> header file. */
#undef HAVE_SYSLOG_H

/* Define to 1 if you have the <sys/int_types.h> header file. */
#undef HAVE_SYS_INT_TYPES_H

/* Define to 1 if you have the <sys/socket.h> header file. */
#undef HAVE_SYS_SOCKET_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/uio.h> header file. */
#undef HAVE_SYS_UIO_H

/* Define to 1 if the system has the type `uint16_t'. */
#define HAVE_UINT16_T 1

/* Define to 1 if the system has the type `uint32_t'. */
#define HAVE_UINT32_T 1

/* Define to 1 if the system has the type `uint64_t'. */
#define HAVE_UINT64_T 1

/* Define to 1 if the system has the type `uint8_t'. */
#define HAVE_UINT8_T 1

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define to 1 if you have the `usleep' function. */
#undef HAVE_USLEEP

/* Define to 1 if you have the <windows.h> header file. */
#define HAVE_WINDOWS_H 1

/* Define to 1 if you have the <winsock2.h> header file. */
#define HAVE_WINSOCK2_H 1

/* Define to use X86 inlined assembly code */
#undef HAVE_X86

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* The size of a `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 4

/* The size of a `unsigned long long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_LONG 8

/* Define to use GDOI. */
#undef SRTP_GDOI

/* Define to compile for kernel contexts. */
#undef SRTP_KERNEL

/* Define to compile for Linux kernel context. */
#undef SRTP_KERNEL_LINUX

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Write errors to this file */
#undef USE_ERR_REPORTING_FILE

/* Define to use syslog logging. */
#undef USE_SYSLOG

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#undef WORDS_BIGENDIAN

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#define inline   // Don't do this, it doesn't link with MSVC
#endif

/* Define to `unsigned' if <sys/types.h> does not define. */
#undef size_t


// Extra bit to original ... disable some warnings
#define LIBSRTP_DISABLED_MSVC_WARNINGS 4018 4100 4214 4244 4389
#pragma warning(disable:LIBSRTP_DISABLED_MSVC_WARNINGS)
#define PACK_STRUCTURE __declspec(align(1))

#ifndef _STDINT
typedef char    int8_t;
typedef short   int16_t;
typedef int     int32_t;
typedef __int64 int64_t;

typedef unsigned char    uint8_t;
typedef unsigned short   uint16_t;
typedef unsigned int     uint32_t;
typedef unsigned __int64 uint64_t;
#endif