/**
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6, SU, CNRS
 */



#pragma once

#include <nrutil.h>
#include <Features.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __KPPV__
#define __KPPV__



void kppv(MeteorROI *stats0, MeteorROI *stat1, int nc0, int nc1, int k);
void mis_en_correspondance(MeteorROI *stats0, MeteorROI *stats1, int nc0, int nc1);
void kppv_routine(MeteorROI *stats0, MeteorROI *stats1, int nc0, int nc1, int k);
void kppv_init(int i0, int i1, int j0, int j1);
void kppv_free(int i0, int i1, int j0, int j1); 



#endif //__KPPV_H__

#ifdef __cplusplus
}
#endif