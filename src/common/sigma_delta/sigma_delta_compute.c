#include <math.h>
#include <stdlib.h>
#include <nrc2.h>

#include "fmdt/macros.h"
#include "fmdt/sigma_delta/sigma_delta_compute.h"

sigma_delta_data_t* sigma_delta_alloc_data(const int i0, const int i1, const int j0, const int j1, const uint8_t vmin,
                                           const uint8_t vmax) {
    sigma_delta_data_t* sd_data = (sigma_delta_data_t*)malloc(sizeof(sigma_delta_data_t));
    sd_data->i0 = i0;
    sd_data->i1 = i1;
    sd_data->j0 = j0;
    sd_data->j1 = j1;
    sd_data->vmin = vmin;
    sd_data->vmax = vmax;
    sd_data->M = ui8matrix(sd_data->i0, sd_data->i1, sd_data->j0, sd_data->j1);
    sd_data->V = ui8matrix(sd_data->i0, sd_data->i1, sd_data->j0, sd_data->j1);
    return sd_data;
}

void sigma_delta_init_data(sigma_delta_data_t* sd_data, const uint8_t** img_in, const int i0, const int i1,
                           const int j0, const int j1) {
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            sd_data->M[i][j] = img_in != NULL ? img_in[i][j] : sd_data->vmax;
            sd_data->V[i][j] = sd_data->vmin;
        }
    }
}

void sigma_delta_free_data(sigma_delta_data_t* sd_data) {
    free_ui8matrix(sd_data->M, sd_data->i0, sd_data->i1, sd_data->j0, sd_data->j1);
    free_ui8matrix(sd_data->V, sd_data->i0, sd_data->i1, sd_data->j0, sd_data->j1);
    free(sd_data);
}

void sigma_delta_compute(sigma_delta_data_t *sd_data, const uint8_t** img_in, uint8_t** img_out, const int i0,
                         const int i1, const int j0, const int j1, const uint8_t N) {
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            const uint8_t pix = img_in[i][j];
            uint8_t m = sd_data->M[i][j];
            if (m < pix)
                m += 1;
            else if (m > pix)
                m -= 1;
            sd_data->M[i][j] = m;

            const uint8_t o = abs(m - pix);

            uint8_t v = sd_data->V[i][j];
            if (v < N * o)
                v += 1;
            else if (v > N * o)
                v -= 1;
            v = MAX(MIN(v, sd_data->vmax), sd_data->vmin);

            sd_data->V[i][j] = v;

            img_out[i][j] = o < v ? 0 : 255;
        }
    }
}
