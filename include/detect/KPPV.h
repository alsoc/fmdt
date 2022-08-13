/**
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6, SU, CNRS
 */

#ifndef __KPPV_H__
#define __KPPV_H__

#include <nrutil.h>

#include "Features.h"

void KPPV_init(int i0, int i1, int j0, int j1);
void KPPV_match(ROI_t* stats0, ROI_t* stats1, int nc0, int nc1, int k);
void KPPV_free(int i0, int i1, int j0, int j1);

#endif //__KPPV_H__
