#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <nrc2.h>

#include "fmdt/defines.h"
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

void LSL_segment_detection(uint32_t* line_er, uint32_t* line_rlc, uint32_t* line_ner, const uint8_t* line,
                           const int j0, const int j1, uint32_t* line_cpy_out) {
    uint32_t j_curr;
    uint32_t j_prev = 0;
    uint32_t f = 0; // Front detection
    uint32_t b = 0;
    uint32_t er = 0;

    for (int j = j0; j <= j1; j++) {
        j_curr = (uint32_t)line[j];
        line_cpy_out[j] = j_curr;
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

void LSL_equivalence_construction(CCL_data_t* data, const uint32_t* line_rlc, uint32_t* line_era,
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
            uint32_t a = data->eq[ea];
            for (erk = er0 + 2; erk <= er1; erk += 2) {
                eak = prevline_era[erk];
                uint32_t ak = data->eq[eak];
                while (ak != data->eq[ak]) {
                    ak = data->eq[ak];
                }
                if (a < ak) {
                    data->eq[eak] = a; // Minimum propagation
                }

                if (a > ak) {
                    a = ak;
                    data->eq[ea] = a;
                    ea = eak;
                }
            }
            line_era[er] = a; // Global minimum
        } else {              // No adjacency -> new label
            line_era[er] = *nea;
            data->eq[*nea] = *nea;
            (*nea)++;
        }
    }
}

uint32_t CCL_LSL_apply(CCL_data_t* data, const uint8_t** img_in, uint32_t** img_out, const int i0, const int i1,
                       const int j0, const int j1) {
    // if ((void*)img_in != (void*)img_out)
    //     for (int i = i0; i <= i1; i++)
    //         memcpy(img_out[i] + j0, img_in[i] + j0, sizeof(uint8_t) * ((j1 - j0) + 1));

    // Step #1 - Segment detection
    for (int i = i0; i <= i1; i++) {
        LSL_segment_detection(data->er[i], data->rlc[i], &data->ner[i], img_in[i], j0, j1, img_out[i]);
    }

    // Step #2 - Equivalence construction
    uint32_t nea = i0;
    uint32_t n = data->ner[i0];
    for (uint32_t k = 0; k < n; k += 2) {
        data->eq[nea] = nea;
        data->era[i0][k + 1] = nea++;
    }
    for (int i = i0 + 1; i <= i1; i++) {
        LSL_equivalence_construction(data, data->rlc[i], data->era[i], data->er[i - 1], data->era[i - 1], data->ner[i],
                                     j0, j1, &nea);
    }

    // Step #3 - Relative to Absolute label conversion

    // Step #4 - Resolution of equivalence classes
    uint32_t trueN = 0;
    for (uint32_t i = 0; i < nea; i++) {
        if (i != data->eq[i]) {
            data->eq[i] = data->eq[data->eq[i]];
        } else {
            data->eq[i] = trueN++;
        }
    }

    // Step #5 - Final image labeling
    for (int i = i0; i <= i1; i++) {
        n = data->ner[i];
        for (uint32_t k = 0; k < n; k += 2) {
            int a = data->rlc[i][k];
            int b = data->rlc[i][k + 1];

            // Step #3 merged with step #5
            uint32_t val = data->era[i][data->er[i][a]];
            val = data->eq[val] + 1;

            for (int j = a; j <= b; j++) {
                img_out[i][j] = (uint32_t)val;
            }
        }
    }

    assert(trueN < MAX_ROI_SIZE);

    return trueN;
}
