/**
 * Copyright (c) 2017-2018, Arthur Hennequin  LIP6, UPMC, CNRS
 * Copyright (c) 2020-2021, Clara Ciocan/ Mathuran Kandeepan LIP6, Sorbonne Universite
 * 
 */

/*
 * LIP6
 */ 

#ifndef __THRESHOLD_H__
#define __THRESHOLD_H__

#include "nrutil.h"
#include "nrtype.h"

#define GRAY_LEVEL 256

void threshold(uint8** m, int i0, int i1, int j0, int j1, uint8 threshold);
void threshold_low(uint8** m, int i0, int i1, int j0, int j1, uint8 threshold);
void threshold_high(uint8** m, int i0, int i1, int j0, int j1, uint8 threshold);
float max_norme(float32**U, float32**V, int i0, int i1, int j0, int j1);

void threshold_norme_compact_bigend(float32**U, float32**V, uint8** out, int w, int h, float threshold);
void threshold_compact_bigend(uint8** in, uint8** out, int w, int h, uint8 threshold);

void threshold_norme_littleend(float32**U, float32**V, uint8** out, int i0, int i1, int j0, int j1, float threshold);
void threshold_norme_compact_littleend(float32**U, float32**V, uint8** out, int w, int h, float threshold);

void pack_ui8vector_bigend(uint8* X1, int src_width, uint8* Y8);
void pack_ui8matrix_bigend(uint8** X1, uint8** Y8, int src_width, int src_height);

void pack_ui8vector_littleend(uint8* X1, int src_width, uint8* Y8);
void pack_ui8matrix_littleend(uint8** X1, int src_height, int src_width, uint8** Y8);

void pack255_ui8vector_littleend(uint8* X1, int src_width, uint8* Y8);
void pack255_ui8matrix_littleend(uint8** X1, int src_height, int src_width, uint8** Y8);

void unpack_ui8matrix_bigend(uint8** in, uint8** out, int w, int h);
void unpack_ui32matrix_bigend(uint8** in, uint32** out, int w, int h);

void unpack255_ui8matrix_littleend(uint8** in, uint8** out, int w, int h);
void unpack_ui8vector_littleend(uint8* X8, int src_width, uint8* Y1);
void unpack_ui8matrix_littleend(uint8** X8, int src_height, int src_width, uint8** Y1);
void unpack_ui32matrix_littlend(uint8** in, uint32** out, int w, int h);


void mask_EDT(uint8** I1, uint8** I2, int i0, int i1, int j0, int j1);
void mask_EDT_ui32matrix(uint32** I1, uint32** I2, int i0, int i1, int j0, int j1);
void mask_EDT_compact(uint8** I1, uint8** I2, int i0, int i1, int j0, int j1);

void histogram(uint8** m, float* h, int i0, int i1, int j0, int j1);
void histogram_uv_norm_sq(uint8** m, float32** U, float32** V, float32* h, int i0, int i1, int j0, int j1);

int otsu_bcv_k(float* h, int k);
int otsu_bcv(float* h, int t0, int t1);
//int otsu_wcv(uint8** m, int i0, int i1, int j0, int j1);

// OpenMP
void unpack_ui8matrix_omp(uint8** in, uint8** out, int w, int h);
void unpack_ui32matrix_omp(uint8** in, uint32** out, int w, int h);

#endif // __THRESHOLD_H__
