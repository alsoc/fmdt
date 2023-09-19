#pragma once

#include "fmdt/sigma_delta/sigma_delta_struct.h"

sigma_delta_data_t* sigma_delta_alloc_data(const int i0, const int i1, const int j0, const int j1, const uint8_t vmin,
                                           const uint8_t vmax);

void sigma_delta_init_data(sigma_delta_data_t* sd_data, const uint8_t** img_in, const int i0, const int i1,
	                       const int j0, const int j1);

void sigma_delta_free_data(sigma_delta_data_t* sd_data);

void sigma_delta_compute(sigma_delta_data_t *sd_data, const uint8_t** img_in, uint8_t** img_out, const int i0,
	                     const int i1, const int j0, const int j1, const uint8_t N);
