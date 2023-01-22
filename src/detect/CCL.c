#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <nrc2.h>

#include "fmdt/CCL.h"

CCL_data_t* CCL_LSL_alloc_and_init_data(int i0, int i1, int j0, int j1) {
    CCL_data_t* data = (CCL_data_t*)malloc(sizeof(CCL_data_t));
    data->i0 = i0;
    data->i1 = i1;
    data->j0 = j0;
    data->j1 = j1;
    long n = (data->i1 - data->i0 + 1) * (data->j1 - data->j0 + 1);
    data->er = ui32matrix(data->i0, data->i1, data->j0, data->j1);
    //data->ea = ui32matrix(data->i0, data->i1, data->j0, data->j1);
    data->era = ui32matrix(data->i0, data->i1, data->j0, data->j1);
    data->rlc = ui32matrix(data->i0, data->i1, data->j0, data->j1);
    data->eq = ui32vector(0, n);
    data->ner = ui32vector(data->i0, data->i1);
    return data;
}

void CCL_LSL_free_data(CCL_data_t* data) {
    long n = (data->i1 - data->i0 + 1) * (data->j1 - data->j0 + 1);
    free_ui32matrix(data->er, data->i0, data->i1, data->j0, data->j1);
    //free_ui32matrix(data->ea, data->i0, data->i1, data->j0, data->j1);
    free_ui32matrix(data->era, data->i0, data->i1, data->j0, data->j1);
    free_ui32matrix(data->rlc, data->i0, data->i1, data->j0, data->j1);
    free_ui32vector(data->eq, 0, n);
    free_ui32vector(data->ner, data->i0, data->i1);
    free(data);
}

void _LSL_segment_detection(uint32_t* line_er, uint32_t* line_rlc, uint32_t* line_ner, const uint8_t* img_line,
                            const int j0, const int j1) {
    uint32_t j_curr;
    uint32_t j_prev = 0;
    uint32_t f = 0; // Front detection
    uint32_t b = 0;
    uint32_t er = 0;

    for (int j = j0; j <= j1; j++) {
        j_curr = (uint32_t)img_line[j];
        f = j_curr ^ j_prev;        // Xor: Front detection
        line_rlc[er] = j - (b & 1); // Begin/End of segment
        b ^= f;                     // Xor: End of segment correction
        er += (f & 1);              // Increment label if front detected
        line_er[j] = er;
        j_prev = j_curr; // Save one load
    }
    j_curr = 0;
    f = j_curr ^ j_prev;
    line_rlc[er] = j1 + 1 - (b & 1);
    er += (f & 1);
    *line_ner = er;
}

void _LSL_equivalence_construction(uint32_t* data_eq, const uint32_t* line_rlc, uint32_t* line_era,
                                   const uint32_t* prevline_er, const uint32_t* prevline_era, const int n, const int x0,
                                   const int x1, uint32_t* nea) {
    int k, er, j0, j1, er0, er1, ea, erk, eak;
    for (k = 0; k < n; k += 2) {
        er = k + 1;

        j0 = line_rlc[k];  // Segment begin
        j1 = line_rlc[er]; // Segment end (k+1)

        // Extends for 8-connected
        if (j0 > x0)
            j0 -= 1;
        if (j1 < x1)
            j1 += 1;

        er0 = prevline_er[j0];
        er1 = prevline_er[j1];

        if ((er0 & 1) == 0) // er0 is even
            er0 += 1;
        if ((er1 & 1) == 0) // er1 is even
            er1 -= 1;

        if (er1 >= er0) { // Adjacency -> connect components
            ea = prevline_era[er0];
            uint32_t a = data_eq[ea];
            for (erk = er0 + 2; erk <= er1; erk += 2) {
                eak = prevline_era[erk];
                uint32_t ak = data_eq[eak];
                while (ak != data_eq[ak]) {
                    ak = data_eq[ak];
                }
                if (a < ak) {
                    data_eq[eak] = a; // Minimum propagation
                }

                if (a > ak) {
                    a = ak;
                    data_eq[ea] = a;
                    ea = eak;
                }
            }
            line_era[er] = a; // Global minimum
        } else {              // No adjacency -> new label
            line_era[er] = *nea;
            data_eq[*nea] = *nea;
            (*nea)++;
        }
    }
}

uint32_t _CCL_LSL_apply(uint32_t** data_er, uint32_t** data_era, uint32_t** data_rlc, uint32_t* data_eq,
                        uint32_t* data_ner, const uint8_t** img, uint32_t** labels, const int i0, const int i1,
                        const int j0, const int j1) {
    for (int i = i0; i <= i1; i++)
        memset(labels[i], 0, sizeof(uint32_t) * ((j1 - j0) + 1));

    // Step #1 - Segment detection
    for (int i = i0; i <= i1; i++) {
        _LSL_segment_detection(data_er[i], data_rlc[i], &data_ner[i], img[i], j0, j1);
    }

    // Step #2 - Equivalence construction
    uint32_t nea = i0;
    uint32_t n = data_ner[i0];
    for (uint32_t k = 0; k < n; k += 2) {
        data_eq[nea] = nea;
        data_era[i0][k + 1] = nea++;
    }
    for (int i = i0 + 1; i <= i1; i++) {
        _LSL_equivalence_construction(data_eq, data_rlc[i], data_era[i], data_er[i - 1], data_era[i - 1], data_ner[i],
                                      j0, j1, &nea);
    }

    // Step #3 - Relative to Absolute label conversion

    // Step #4 - Resolution of equivalence classes
    uint32_t trueN = 0;
    for (uint32_t i = 0; i < nea; i++) {
        if (i != data_eq[i]) {
            data_eq[i] = data_eq[data_eq[i]];
        } else {
            data_eq[i] = trueN++;
        }
    }

    // Step #5 - Final image labeling
    for (int i = i0; i <= i1; i++) {
        n = data_ner[i];
        for (uint32_t k = 0; k < n; k += 2) {
            int a = data_rlc[i][k];
            int b = data_rlc[i][k + 1];

            // Step #3 merged with step #5
            uint32_t val = data_era[i][data_er[i][a]];
            val = data_eq[val] + 1;

            for (int j = a; j <= b; j++) {
                labels[i][j] = (uint32_t)val;
            }
        }
    }

    return trueN;
}

uint32_t CCL_LSL_apply(CCL_data_t* data, const uint8_t** img, uint32_t** labels) {
    return _CCL_LSL_apply(data->er, data->era, data->rlc, data->eq, data->ner, img, labels, data->i0, data->i1,
                          data->j0, data->j1);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#include <flsl-lib.h>

enum ccl_impl_e CCL_str_to_enum(const char* str) {
    if (strcmp(str, "LSLH") == 0) {
        return LSLH;
    } else if (strcmp(str, "LSLM") == 0) {
        return LSLM;
    } else {
        fprintf(stderr, "(EE) 'CCL_str_to_enum()' failed, unknown input ('%s').\n", str);
        exit(-1);
    }
}

CCL_gen_data_t* CCL_alloc_and_init_data(const enum ccl_impl_e impl, const int i0, const int i1, const int j0,
                                        const int j1) {
    CCL_gen_data_t* data = (CCL_gen_data_t*)malloc(sizeof(CCL_gen_data_t));
    data->impl = impl;
    switch (data->impl) {
        case LSLH:
            data->metadata = (void*)CCL_LSL_alloc_and_init_data(i0, i1, j0, j1);
            break;
        case LSLM:
            // FLSL_Data* FLSL_FSM(long i0, long i1, long j0, long j1);
            data->metadata = (void*)FLSL_FSM((long)i0, (long)i1, (long)j0, (long)j1);
            break;
        default:
            fprintf(stderr, "(EE) This should never happen.\n");
            exit(-1);
    }
    return data;
}

uint32_t CCL_apply(CCL_gen_data_t* data, const uint8_t** img, uint32_t** labels) {
    switch (data->impl) {
        case LSLH:
            return CCL_LSL_apply((CCL_data_t*)data->metadata, img, labels);
            break;
        case LSLM: // /!\ SIMD LSL versions require {0, 255} 'img' to work!!
            // int FLSL_FSM_start(uint8** img, sint32** labels, FLSL_Data* metadata);
            return (uint32_t)FLSL_FSM_start((uint8_t**)img, (int32_t**)labels, (FLSL_Data*)data->metadata);
            break;
        default:
            fprintf(stderr, "(EE) This should never happen.\n");
            exit(-1);
    }
}

void CCL_free_data(CCL_gen_data_t* data) {
        switch (data->impl) {
        case LSLH:
            CCL_LSL_free_data((CCL_data_t*)data->metadata);
            break;
        case LSLM:
            // void FLSL_FSM_free(FLSL_Data* metadata);
            FLSL_ER_free((FLSL_Data*)data->metadata);
            break;
        default:
            fprintf(stderr, "(EE) This should never happen.\n");
            exit(-1);
    }
    free(data);
}
