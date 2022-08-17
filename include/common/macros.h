/* --------------------- */
/* --- macro_debug.h --- */
/* --------------------- */

#pragma once

#include <stdint.h>

// ENABLE_DEBUG : macro definissant le fonctionnement general des macros de debug

#ifdef ENABLE_DEBUG

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

#endif // ENABLE_DEBUG

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif
#define CLAMP(x, a, b) MIN(MAX(x, a), b)
#define SWAP_STATS(dest, src, n_src)                                                                                   \
    for (int i = 1; i <= n_src; i++) {                                                                                 \
        dest[i] = src[i];                                                                                              \
    }
#define SWAP_UI8(a, b)                                                                                                 \
    {                                                                                                                  \
        uint8_t** tmp = a;                                                                                               \
        a = b;                                                                                                         \
        b = tmp;                                                                                                       \
    }

