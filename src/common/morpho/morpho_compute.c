#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <nrc2.h>

#include "fmdt/macros.h"
#include "fmdt/morpho/morpho_compute.h"

morpho_data_t* morpho_alloc_data(const int i0, const int i1, const int j0, const int j1) {
    morpho_data_t* morpho_data = (morpho_data_t*)malloc(sizeof(morpho_data_t));
    morpho_data->i0 = i0;
    morpho_data->i1 = i1;
    morpho_data->j0 = j0;
    morpho_data->j1 = j1;
    morpho_data->img_tmp = ui8matrix(morpho_data->i0, morpho_data->i1, morpho_data->j0, morpho_data->j1);
    return morpho_data;
}

void morpho_init_data(morpho_data_t* morpho_data) {
    zero_ui8matrix(morpho_data->img_tmp , morpho_data->i0, morpho_data->i1, morpho_data->j0, morpho_data->j1);
}

void morpho_free_data(morpho_data_t* morpho_data) {
    free_ui8matrix(morpho_data->img_tmp, morpho_data->i0, morpho_data->i1, morpho_data->j0, morpho_data->j1);
    free(morpho_data);
}

void morpho_compute_erosion3(const uint8_t** img_in, uint8_t** img_out, const int i0, const int i1, const int j0,
                             const int j1) {
    assert(img_in != NULL);
    assert(img_out != NULL);
    assert(img_in != img_out);
    for (int i = i0 + 1; i <= i1 - 1; i++) {
        for (int j = j0 + 1; j <= j1 - 1; j++) {
            uint8_t c0 = img_in[i - 1][j - 1] & img_in[i - 1][j] & img_in[i - 1][j + 1];
            uint8_t c1 = img_in[i + 0][j - 1] & img_in[i + 0][j] & img_in[i + 0][j + 1];
            uint8_t c2 = img_in[i + 1][j - 1] & img_in[i + 1][j] & img_in[i + 1][j + 1];
            img_out[i][j] = c0 & c1 & c2;
        }
    }
}

void morpho_compute_dilatation3(const uint8_t** img_in, uint8_t** img_out, const int i0, const int i1, const int j0,
                                const int j1) {
    assert(img_in != NULL);
    assert(img_out != NULL);
    assert(img_in != img_out);
    for (int i = i0 + 1; i <= i1 - 1; i++) {
        for (int j = j0 + 1; j <= j1 - 1; j++) {
            uint8_t c0 = img_in[i - 1][j - 1] | img_in[i - 1][j] | img_in[i - 1][j + 1];
            uint8_t c1 = img_in[i + 0][j - 1] | img_in[i + 0][j] | img_in[i + 0][j + 1];
            uint8_t c2 = img_in[i + 1][j - 1] | img_in[i + 1][j] | img_in[i + 1][j + 1];
            img_out[i][j] = c0 | c1 | c2;
        }
    }
}

void morpho_compute_opening3(morpho_data_t* morpho_data, const uint8_t** img_in, uint8_t** img_out, const int i0,
                             const int i1, const int j0, const int j1) {
    assert(img_in != NULL);
    assert(img_out != NULL);
    morpho_compute_erosion3   ((const uint8_t**)img_in, morpho_data->img_tmp, i0, i1, j0, j1);
    morpho_compute_dilatation3((const uint8_t**)morpho_data->img_tmp, img_out, i0, i1, j0, j1);
}

void morpho_compute_closing3(morpho_data_t* morpho_data, const uint8_t** img_in, uint8_t** img_out, const int i0,
                             const int i1, const int j0, const int j1) {
    assert(img_in != NULL);
    assert(img_out != NULL);
    morpho_compute_dilatation3((const uint8_t**)img_in, morpho_data->img_tmp, i0, i1, j0, j1);
    morpho_compute_erosion3   ((const uint8_t**)morpho_data->img_tmp, img_out, i0, i1, j0, j1);
}
