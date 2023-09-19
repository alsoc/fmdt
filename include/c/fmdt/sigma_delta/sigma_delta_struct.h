#pragma once

typedef struct {
	int i0;
	int i1;
	int j0;
	int j1;
	uint8_t vmin;
	uint8_t vmax;
    uint8_t **M;
    uint8_t **V;
} sigma_delta_data_t;
