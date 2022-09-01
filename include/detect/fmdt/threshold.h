/**
 * Copyright (c) 2017-2018, Arthur Hennequin  LIP6, UPMC, CNRS
 * Copyright (c) 2020-2021, Clara Ciocan/ Mathuran Kandeepan LIP6, Sorbonne Universite
 *
 */

#pragma once

#include <stdint.h>

void threshold(const uint8_t** m_in, uint8_t** m_out, const int i0, const int i1, const int j0, const int j1,
               const uint8_t threshold);
void threshold_low(const uint8_t** m_in, uint8_t** m_out, const int i0, const int i1, const int j0, const int j1,
                   const uint8_t threshold);
void threshold_high(const uint8_t** m_in, uint8_t** m_out, const int i0, const int i1, const int j0, const int j1,
                    const uint8_t threshold);
// float max_norme(float** U, float** V, int i0, int i1, int j0, int j1);
// void threshold_norme_compact_bigend(float** U, float** V, uint8_t** out, int w, int h, float threshold);
// void threshold_compact_bigend(uint8_t** in, uint8_t** out, int w, int h, uint8_t threshold);
// void threshold_norme_littleend(float** U, float** V, uint8_t** out, int i0, int i1, int j0, int j1, float threshold);
// void threshold_norme_compact_littleend(float** U, float** V, uint8_t** out, int w, int h, float threshold);
// void pack_ui8vector_bigend(uint8_t* X1, int src_width, uint8_t* Y8);
// void pack_ui8matrix_bigend(uint8_t** X1, uint8_t** Y8, int src_width, int src_height);
// void pack_ui8vector_littleend(uint8_t* X1, int src_width, uint8_t* Y8);
// void pack_ui8matrix_littleend(uint8_t** X1, int src_height, int src_width, uint8_t** Y8);
// void pack255_ui8vector_littleend(uint8_t* X1, int src_width, uint8_t* Y8);
// void pack255_ui8matrix_littleend(uint8_t** X1, int src_height, int src_width, uint8_t** Y8);
// void unpack_ui8matrix_bigend(uint8_t** in, uint8_t** out, int w, int h);
// void unpack_ui32matrix_bigend(uint8_t** in, uint32_t** out, int w, int h);
// void unpack255_ui8matrix_littleend(uint8_t** in, uint8_t** out, int w, int h);
// void unpack_ui8vector_littleend(uint8_t* X8, int src_width, uint8_t* Y1);
// void unpack_ui8matrix_littleend(uint8_t** X8, int src_height, int src_width, uint8_t** Y1);
// void unpack_ui32matrix_littlend(uint8_t** in, uint32_t** out, int w, int h);
// void mask_EDT(uint8_t** I1, uint8_t** I2, int i0, int i1, int j0, int j1);
// void mask_EDT_ui32matrix(uint32_t** I1, uint32_t** I2, int i0, int i1, int j0, int j1);
// void mask_EDT_compact(uint8_t** I1, uint8_t** I2, int i0, int i1, int j0, int j1);
// void histogram(uint8_t** m, float* h, int i0, int i1, int j0, int j1);
// void histogram_uv_norm_sq(uint8_t** m, float** U, float** V, float* h, int i0, int i1, int j0, int j1);
// int otsu_bcv_k(float* h, int k);
// int otsu_bcv(float* h, int t0, int t1);
// int otsu_wcv(uint8_t** m, int i0, int i1, int j0, int j1);

// OpenMP
// void unpack_ui8matrix_omp(uint8_t** in, uint8_t** out, int w, int h);
// void unpack_ui32matrix_omp(uint8_t** in, uint32_t** out, int w, int h);
