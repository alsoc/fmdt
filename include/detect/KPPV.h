/**
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6, SU, CNRS
 */

#pragma once

#include <stdint.h>

#include "features.h"

typedef struct {
    int i0, i1, j0, j1;
    uint32_t** nearest;
    float** distances;
    uint32_t* conflicts;
} KKPV_data_t;

KKPV_data_t* KPPV_init(int i0, int i1, int j0, int j1);
void KPPV_match(KKPV_data_t* data, ROI_t* stats0, ROI_t* stats1, int nc0, int nc1, int k);
void KPPV_free(KKPV_data_t* data);

// void KPPV_save_asso(const char* filename, uint32_t** nearest, float** distances, int nc0, ROI_t* stats);
// void KPPV_save_asso_VT(const char* filename, int nc0, ROI_t* stats, int frame);
// void KPPV_save_conflicts(const char* filename, uint32_t* conflicts, uint32_t** nearest, float** distances,
//                          int n_asso, int n_conflict);
void KPPV_save_asso_conflicts(const char* filename, int frame, KKPV_data_t* data, int n_asso, int n_conflict,
                              ROI_t* stats0, ROI_t* stats1, track_t* tracks, int n_tracks);

