/**
 * Copyright (c) 2017-2018, Arthur Hennequin
 * LIP6, UPMC, CNRS
 */

/*********************************
 * Connected Components Labeling *
 *********************************/

#pragma once

#include <stdint.h>

typedef struct {
    int i0, i1, j0, j1;
    uint32_t** er;  // Relative labels
    //uint32_t** ea;  // Absolute labels
    uint32_t** era; // Relative/Absolute labels equivalences;
    uint32_t** rlc; // Run-length coding
    uint32_t* eq;   // Table d'équivalence
    uint32_t* ner;  // Number of relative labels
} CCL_data_t;

CCL_data_t* CCL_LSL_alloc_and_init(int i0, int i1, int j0, int j1);
uint32_t CCL_LSL_apply(CCL_data_t *data, uint32_t** img, int i0, int i1, int j0, int j1);
void CCL_LSL_free(CCL_data_t* data);
