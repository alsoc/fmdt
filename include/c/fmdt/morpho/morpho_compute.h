#pragma once

#include "fmdt/morpho/morpho_struct.h"

morpho_data_t* morpho_alloc_data(const int i0, const int i1, const int j0, const int j1);

void morpho_init_data(morpho_data_t* morpho_data);

void morpho_free_data(morpho_data_t* morpho_data);

void morpho_compute_erosion3(const uint8_t** img_in, uint8_t** img_out, const int i0, const int i1, const int j0,
                             const int j1);

void morpho_compute_dilatation3(const uint8_t** img_in, uint8_t** img_out, const int i0, const int i1, const int j0,
                                const int j1);

void morpho_compute_opening3(morpho_data_t* morpho_data, const uint8_t** img_in, uint8_t** img_out, const int i0,
                             const int i1, const int j0, const int j1);

void morpho_compute_closing3(morpho_data_t* morpho_data, const uint8_t** img_in, uint8_t** img_out, const int i0,
                             const int i1, const int j0, const int j1);
