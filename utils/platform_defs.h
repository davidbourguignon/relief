#ifndef __PLATFORM_DEFS_H__
#define __PLATFORM_DEFS_H__

/*
 * The following user-defined environment variables are used:
 * WIN32_BUILD_LIBRARY when building a dynamic/static library under MS Windows.
 * WIN32_DLL when building/using a dynamically linked library under MS Windows.
 *
 * Warning: There might be a smarter way to do this using existing
 *          environment variables, such as _DLL or _STATIC_CPPLIB
 *          (see for example Microsoft SDK\src\crt\internal.h).
 */

/* Headers and environment variables */
#if defined(_WIN32) && defined(_MSC_VER)
#  define WIN32_LEAN_AND_MEAN
#  define _USE_MATH_DEFINES
#  define NOMINMAX
#  include <windows.h>
#  undef HAVE_UNISTD_H
#elif defined(_WIN32) && defined(__MINGW32__)
#  error ERROR: mswin-mingw OS-compiler combination not tested!
#elif defined(__linux__) && defined(__GNUC__)
#  define HAVE_UNISTD_H
#elif defined(__sgi) && !defined(__GNUC__) /* Assume the mipspro compiler */
#  error ERROR: sgi IRIX-mipspro OS-compiler combination not tested!
#else
#  error ERROR: Unknown OS-compiler combination!
#endif

/* C standard types and functions */
#ifndef __cplusplus
#if defined(_WIN32) && defined(_MSC_VER)
   typedef int bool;
#  define true 1
#  define false 0
#  define fabsf(x) ((float)(fabs)((double)(x)))
#  define sqrtf(x) ((float)(sqrt)((double)(x)))
#elif defined(_WIN32) && defined(__MINGW32__)
#  error ERROR: mswin-mingw OS-compiler combination not tested!
#elif defined(__linux__) && defined(__GNUC__)
#  include <stdbool.h>
#elif defined(__sgi) && !defined(__GNUC__) /* Assume the mipspro compiler */
#  error ERROR: sgi IRIX-mipspro OS-compiler combination not tested!
#else
#  error ERROR: Unknown OS-compiler combination!
#endif
#endif

/* C/C++ keywords */
#if defined(_WIN32) && defined(_MSC_VER)
#  if defined(WIN32_BUILD_LIBRARY)
#    if defined(WIN32_DLL)
#      define EXTERND __declspec(dllexport)
#    else /* !WIN32_DLL == WIN32_SLL */
#      define EXTERND extern
#    endif
#  else
#    if defined(WIN32_DLL)
#      define EXTERND __declspec(dllimport)
#    else /* !WIN32_DLL == WIN32_SLL */
#      define EXTERND extern
#    endif
#  endif
#  define INLINED static __inline
#  define CALLBACKD CALLBACK
#elif defined(_WIN32) && defined(__MINGW32__)
#  error ERROR: mswin-mingw OS-compiler combination not tested!
#elif defined(__linux__) && defined(__GNUC__)
#  define EXTERND extern
#  define INLINED static inline
#  define CALLBACKD
#elif defined(__sgi) && !defined(__GNUC__) /* Assume the mipspro compiler */
#  error ERROR: sgi IRIX-mipspro OS-compiler combination not tested!
#else
#  error ERROR: Unknown OS-compiler combination!
#endif

/* CPU endianess */
#if defined(_WIN32) || defined(__linux__) /* Assume Intel x86 architecture */
#  define IS_LITTLE_ENDIAN
#elif defined(__sgi) /* Assume sgi mips architecture */
#  define IS_BIG_ENDIAN
#else
#  error ERROR: Unknown OS-architecture combination!
#endif

#endif /* !__PLATFORM_DEFS_H__ */
