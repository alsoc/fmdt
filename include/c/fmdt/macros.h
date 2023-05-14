/*!
 * \file
 * \brief Define some basic macros for debugging and Mathematics operations.
 */

#pragma once

#include <stdint.h>
#include <sys/time.h>

// FMDT_ENABLE_DEBUG : macro definissant le fonctionnement general des macros de debug

#ifdef FMDT_ENABLE_DEBUG

// macro de debug
#ifndef VERBOSE
#define VERBOSE(X) X
#endif
#define PUTS(str) fprintf(stderr, "(DBG) %s\n", str)
#define CR putchar('\n');
#define SHOWNAME(X) #X

#define IDISP(x) fprintf(stderr, "(DBG) %s = %3d\n", #x, x)
#define FDISP(x) fprintf(stderr, "(DBG) %s = %3f\n", #x, x)
#define DISP(x) fprintf(stderr, "(DBG) %s = %s\n", #x, x)

#else

#ifndef VERBOSE
#define VERBOSE(X)
#endif
#define PUTS(str)
#define CR putchar('\n');
#define SHOWNAME(X) #X // pas d'appel en mode release

#define DISP(x)
#define FDISP(x)
#define IDISP(x)

#endif // FMDT_ENABLE_DEBUG

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif
#define CLAMP(x, a, b) MIN(MAX(x, a), b)

#define TIME_POINT(name) struct timeval t_##name; \
                         gettimeofday(&t_##name, NULL); \
                         const double t_##name##_us = (double)(t_##name.tv_sec * 1e6 + t_##name.tv_usec);
#define TIME_ELAPSED_US(name1, name2) (t_##name2##_us - t_##name1##_us)
#define TIME_ELAPSED_MS(name1, name2) (t_##name2##_us - t_##name1##_us) * 1e-3
#define TIME_ELAPSED_S(name1, name2) (t_##name2##_us - t_##name1##_us) * 1e-6
#define TIME_ELAPSED_SEC(name1, name2) TIME_ELAPSED_S(name1, name2)
