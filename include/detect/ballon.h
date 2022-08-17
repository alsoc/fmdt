/**
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#pragma once

#include <stdint.h>

typedef struct {
    uint8_t **I0, **I1; // frame t e t+1
    uint8_t **SB, **SM, **SH; // hysteresis
    uint8_t** I0_1; // EDT
    uint32_t** I32;
    uint32_t** I32_1; // EDT
    uint32_t **SB32, **SM32, **SH32;
} ballon_t;

ballon_t* ballon_alloc(int i0, int i1, int j0, int j1, int b);
void ballon_init(ballon_t* ballon, int i0, int i1, int j0, int j1, int b);
void ballon_free(ballon_t* ballon, int i0, int i1, int j0, int j1, int b);
