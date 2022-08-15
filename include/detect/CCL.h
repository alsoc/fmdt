/**
 * Copyright (c) 2017-2018, Arthur Hennequin
 * LIP6, UPMC, CNRS
 */

/*********************************
 * Connected Components Labeling *
 *********************************/

#ifndef __CCL_H__
#define __CCL_H__

#include <stdint.h>

void CCL_LSL_init(int i0, int i1, int j0, int j1);
uint32_t CCL_LSL_apply(uint32_t** img, int i0, int i1, int j0, int j1);
void CCL_LSL_free(int i0, int i1, int j0, int j1);

#endif // __CCL_H__
