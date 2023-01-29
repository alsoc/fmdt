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

/* LSLH = LSL Hennequin, LSLM = LSL Maurice */
enum ccl_impl_e { LSLH = 0, LSLM };

typedef struct {
    enum ccl_impl_e impl;
    void* metadata;
} CCL_gen_data_t;
