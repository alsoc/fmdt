/**
 * Copyright (c) 2021-2022, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2021-2022, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#include <stdlib.h>
#include <nrc2.h>

#include "ballon.h"

ballon_t* ballon_alloc(int i0, int i1, int j0, int j1, int b) {
    ballon_t* ballon = (ballon_t*)malloc(sizeof(ballon_t));
    ballon->I0 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    ballon->I1 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    ballon->SB = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    ballon->SM = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    ballon->SH = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    ballon->I32 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    ballon->SH32 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    ballon->SM32 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    ballon->SB32 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    return ballon;
}

void ballon_init(ballon_t* ballon, int i0, int i1, int j0, int j1, int b) {
    zero_ui8matrix(ballon->I0, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(ballon->I1, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(ballon->SB, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(ballon->SM, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(ballon->SH, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(ballon->I32, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(ballon->SB32, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(ballon->SM32, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(ballon->SH32, i0 - b, i1 + b, j0 - b, j1 + b);
}

void ballon_free(ballon_t* ballon, int i0, int i1, int j0, int j1, int b) {
    free_ui8matrix(ballon->I0, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(ballon->I1, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(ballon->SB, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(ballon->SM, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(ballon->SH, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(ballon->I32, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(ballon->SB32, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(ballon->SM32, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(ballon->SH32, i0 - b, i1 + b, j0 - b, j1 + b);
    free(ballon);
}
