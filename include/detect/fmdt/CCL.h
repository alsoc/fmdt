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

CCL_data_t* CCL_LSL_alloc_and_init_data(int i0, int i1, int j0, int j1);
uint32_t CCL_LSL_apply(CCL_data_t *data, const uint8_t** img_in, uint32_t** img_out, const int i0, const int i1,
                       const int j0, const int j1);
void CCL_LSL_free_data(CCL_data_t* data);
