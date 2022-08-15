/* --------------------- */
/* --- macro_debug.h --- */
/* --------------------- */

#ifndef __MACROS_H__
#define __MACROS_H__

// ENABLE_DEBUG : macro definissant le fonctionnement general des macros de debug

// ----------------------------------------------------- //
// -- ne rien ecrire en desous de cette ligne ---------- //
// ----------------------------------------------------- //

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

// Timing
#define BENCH(X, n, cpp)                                                                                               \
    do {                                                                                                               \
        struct_timespec t0, t1;                                                                                        \
        double dt;                                                                                                     \
        int iter = 100;                                                                                                \
        t0 = getCycles();                                                                                              \
        for (int k = 0; k < iter; k++) {                                                                               \
            X;                                                                                                         \
        }                                                                                                              \
        t1 = getCycles();                                                                                              \
        dt = diff_ns(t0, t1);                                                                                          \
        cpp = dt / (iter * n * n);                                                                                     \
    } while (0)
#define BENCH_TOTAL(X, n, cpp)                                                                                         \
    do {                                                                                                               \
        struct_timespec t0, t1;                                                                                        \
        double dt;                                                                                                     \
        int iter = 1;                                                                                                  \
        t0 = getCycles();                                                                                              \
        for (int k = 0; k < iter; k++) {                                                                               \
            X;                                                                                                         \
        }                                                                                                              \
        t1 = getCycles();                                                                                              \
        dt = diff_ns(t0, t1);                                                                                          \
        cpp = dt / (iter * n);                                                                                         \
    } while (0)

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
        uint8** tmp = a;                                                                                               \
        a = b;                                                                                                         \
        b = tmp;                                                                                                       \
    }

#endif // __MACROS_H__
