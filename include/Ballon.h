/**
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */
#include "nrutil.h"
#include "Args.h"
#include "Video.h"
#include "CCL.h"
#include "Features.h"
#include "Threshold.h"
#include "Tracking.h"
#include "macro_debug.h"

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef __BALLON_H__
#define __BALLON_H__

typedef struct {
    uint8 **I0, **I1;       // frame t e t+1
    uint8 **SB, **SM, **SH; // hysteresis
    uint8 **I0_1; // EDT


    uint32 **I32;
    uint32 **I32_1; // EDT
    uint32 **SB32, **SM32, **SH32;
} Ballon; 

Ballon* allocBallon(int i0, int i1, int j0, int j1, int b);
void initBallon (Ballon *ballon, int i0, int i1, int j0, int j1, int b);
void freeBallon (Ballon *ballon, int i0, int i1, int j0, int j1, int b);

#endif