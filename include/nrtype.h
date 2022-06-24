/* ---------------- */
/* --- nrtype.h --- */
/* ---------------- */

/*
 * Copyright (c) 2000 - 2013, Lionel Lacassagne, All rights reserved
 * University of Paris Sud, Laboratoire de Recherche en Informatique 
 */

#ifndef __NRTYPE_H__
#define __NRTYPE_H__

#include <stdint.h>
#include <inttypes.h> // for new types ?

//#pragma message("include light version of def.h")

/*
// old C-ANSI definition
typedef unsigned char byte;

typedef unsigned char uint8;
typedef   signed char sint8;

typedef unsigned short uint16;
typedef   signed short sint16;

typedef unsigned int uint32;
typedef   signed int sint32;

typedef unsigned long long int uint64;
typedef   signed long long int sint64;
*/

// new C99 definition

typedef uint8_t bit_t;
typedef uint8_t byte;

typedef uint8_t uint8;
typedef  int8_t sint8;

typedef uint16_t uint16;
typedef  int16_t sint16;

typedef uint32_t uint32;
typedef  int32_t sint32;

typedef uint64_t uint64;
typedef  int64_t sint64;

typedef float         float32;
typedef double        float64;
typedef struct { uint8 r; uint8 g; uint8 b;} rgb8;

typedef int BOOL;

#endif // __NRTYPE_H__
