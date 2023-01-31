#pragma once

#include <stdint.h>

#include "fmdt/features/features_struct.h"

void image_write_PNM_row(const uint8_t* line, const int width, FILE* file);
void image_save_frame_threshold(const char* filename, uint8_t** I0, uint8_t** I1, int i0, int i1, int j0, int j1);
void image_save_frame_quad(const char* filename, uint8_t** I0, uint8_t** I1, uint32_t** I2, uint32_t** I3, int nbLabel,
                           RoI_t* stats, int i0, int i1, int j0, int j1);
void image_save_frame_quad_hysteresis(const char* filename, uint8_t** I0, uint32_t** SH, uint32_t** SB, uint32_t** Y,
                                      int i0, int i1, int j0, int j1);