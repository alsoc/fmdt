#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <nrc2.h>
#include <vec.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/macros.h"
#include "fmdt/tools.h"

void tools_create_folder(const char* folder_path) {
    struct stat status = {0};
    if (stat(folder_path, &status) == -1)
        mkdir(folder_path, 0700);
}

void tools_copy_ui8matrix_ui8matrix(const uint8_t** X, const int i0, const int i1, const int j0, const int j1,
                                    uint8_t** Y) {
    for (int i = i0; i <= i1; i++)
        memcpy(Y[i] + j0, X[i] + j0, sizeof(uint8_t) * ((j1 - j0) + 1));
}

void tools_convert_ui8vector_ui32vector(const uint8_t* X, const long nl, const long nh, uint32_t* Y) {
    for (long i = nl; i <= nh; i++)
        Y[i] = (uint32_t)X[i];
}

// void tools_convert_ui8matrix_ui32matrix(const uint8** X, const int nrl, const int nrh, const int ncl, const int nch,
//                                         uint32** Y) {
//     for (long i = nrl; i <= nrh; i++)
//         tools_convert_ui8vector_ui32vector(X[i], ncl, nch, Y[i]);
// }

void tools_convert_ui8matrix_ui32matrix(const uint8_t** X, const int nrl, const int nrh, const int ncl, const int nch,
                                        uint32_t** Y) {
    for (long i = nrl; i <= nrh; i++)
        for (long j = ncl; j <= nch; j++)
            Y[i][j] = (uint32_t)X[i][j];
}

#define TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y) \
    assert(X != NULL);                            \
    assert(Y != NULL);                            \
    assert(i0 <= 0);                              \
    assert(j0 <= 0);                              \
    assert(i1 > i0 && i1 > 0);                    \
    assert(j1 > j0 && j1 > 0);                    \
    Y[i0] = X - j0;                               \
    for (int i = i0 + 1; i <= i1 ; i++)           \
        Y[i] = Y[i - 1] + ((j1 - j0) + 1);

void tools_linear_2d_nrc_ui8matrix(const uint8_t* X, const int i0, const int i1, const int j0, const int j1,
                         const uint8_t** Y) {
    TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y)
}

void tools_linear_2d_nrc_ui32matrix(const uint32_t* X, const int i0, const int i1, const int j0, const int j1,
                         const uint32_t** Y) {
    TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y)
}

void tools_linear_2d_nrc_f32matrix(const float* X, const int i0, const int i1, const int j0, const int j1,
                                   const float** Y) {
    TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y)
}

void tools_linear_2d_nrc_rgb8matrix(const rgb8_t* X, const int i0, const int i1, const int j0, const int j1,
                                    const rgb8_t** Y) {
    TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y)
}

int tools_is_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

void tools_save_max(const char* filename, uint8_t** I, int i0, int i1, int j0, int j1) {
    uint8_t m;
    uint8_t* res = ui8vector(i0, i1);

    zero_ui8vector(res, i0, i1);

    for (int i = i0; i <= i1; i++) {
        m = 0;
        for (int j = j0; j <= j1; j++) {
            m = MAX(m, I[i][j]);
        }
        res[i] = m;
    }

    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = i1; i >= i0; i--) {
        fprintf(f, "%4d\t%4d\n", i, res[i]);
    }
    fclose(f);
}

// void tools_filter_speed_binarize(uint32_t** in, int i0, int i1, int j0, int j1, uint8_t** out, RoI_t* stats) {
//     for (int i = i0; i <= i1; i++)
//         for (int j = j0; j <= j1; j++)
//             out[i][j] = stats[in[i][j]].S ? 0xFF : 0;
// }

void tools_convert_char_int_cvector(const char* arg, vec_int *res) {
    char *saveptr1;
    char arg_cpy[2048];
    strncpy(arg_cpy, arg, sizeof(arg_cpy));
    arg_cpy[sizeof(arg_cpy) - 1] = 0;
    char *cur_arg = arg_cpy;
    cur_arg = strtok_r(cur_arg, "[,]", &saveptr1);
    
    do {
        vector_add(res, atoi(cur_arg));
    } while((cur_arg = strtok_r(NULL, "[,]", &saveptr1)) != NULL);
}

void tools_int_cvector_print(FILE* stream, vec_int tab) {
    int size = vector_size(tab);
    fprintf(stream, "[");
    for (int i = 0; i < size - 1; i++) {
        fprintf(stream, "%d, ", tab[i]);
    }
    fprintf(stream, "%d]", tab[size - 1]);
}