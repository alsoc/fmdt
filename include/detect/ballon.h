/**
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#ifndef __BALLON_H__
#define __BALLON_H__

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <nrutil.h>

#include "args.h"
#include "CCL.h"
#include "features.h"
#include "threshold.h"
#include "tracking.h"
#include "video.h"
#include "macros.h"

#define SIZE_MAX_METEORROI 18000
#define SIZE_MAX_TRACKS 1000
#define TOLERANCE_DISTANCEMIN 20
#define SIZE_MAX_KPPV 200

typedef struct {
    uint8 **I0, **I1; // frame t e t+1
    uint8 **SB, **SM, **SH; // hysteresis
    uint8** I0_1; // EDT
    uint32** I32;
    uint32** I32_1; // EDT
    uint32 **SB32, **SM32, **SH32;
} ballon_t;

ballon_t* ballon_alloc(int i0, int i1, int j0, int j1, int b);
void ballon_init(ballon_t* ballon, int i0, int i1, int j0, int j1, int b);
void ballon_free(ballon_t* ballon, int i0, int i1, int j0, int j1, int b);

#endif // __BALLON_H__
