/* --------------------- */
/* --- macro_debug.h --- */
/* --------------------- */

#ifndef __MACRO_DEBUG_H__
#define __MACRO_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

// ENABLE_DEBUG : macro definissant le fonctionnement general des macros de debug
// si macro active, debug
// si macro inactive, release
// #define ENABLE_CONTROL : macro pour activer le controle d'acces

// #define DEBUG
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


#define idisp(x) fprintf(stderr, "(DBG) %s = %3d\n", #x, x)
#define fdisp(x) fprintf(stderr, "(DBG) %s = %3f\n", #x, x)
#define disp(x) fprintf(stderr, "(DBG) %s = %s\n", #x, x)

#else

#ifndef VERBOSE
#define VERBOSE(X)
#endif
#define PUTS(str) 
#define CR putchar('\n');
#define SHOWNAME(X) #X          // pas d'appel en mode release

#define disp(x)
#define fdisp(x)
#define idisp(x)

#endif // ENABLE_DEBUG

#ifdef __cplusplus
}
#endif

#endif // __MACRO_DEBUG_H__
