/**
 * Copyright (c) 2017-2018, Arthur Hennequin
 * LIP6, UPMC, CNRS
 */

#include "threshold.h"

#define GRAY_LEVEL 256

void threshold(uint8_t** m, int i0, int i1, int j0, int j1, uint8_t threshold) {
    int i, j;
    for (i = i0; i <= i1; i++) {
        for (j = j0; j <= j1; j++) {
            m[i][j] = (m[i][j] < threshold) ? 0 : 255;
        }
    }
}

void threshold_high(uint8_t** m, int i0, int i1, int j0, int j1, uint8_t th) { threshold(m, i0, i1, j0, j1, th); }

void threshold_low(uint8_t** m, int i0, int i1, int j0, int j1, uint8_t threshold) {
    int i, j;
    for (i = i0; i <= i1; i++) {
        for (j = j0; j <= j1; j++) {
            m[i][j] = (m[i][j] > threshold) ? 0 : 255;
        }
    }
}

float max_norme(float** U, float** V, int i0, int i1, int j0, int j1) {
    float maxi = 0;
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            float norme = U[i][j] * U[i][j] + V[i][j] * V[i][j];
            if (maxi < norme)
                maxi = norme;
        }
    }
    return maxi;
}

void threshold_norme_compact_bigend(float** U, float** V, uint8_t** out, int w, int h, float threshold) {
    float t2 = threshold * threshold; // comparaison avec le carre de la norme

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            uint8_t val = 0;
            uint8_t mask = 0x80; // Arthur en big endian.

            // boucle sur les 8 bits
            for (int k = 0; k < 8; k++) {

                float u = U[i][(j << 3) + k];
                float v = V[i][(j << 3) + k];
                float norme2 = u * u + v * v; // norme au carre
                val |= (norme2 < t2) ? 0 : mask;

                mask = mask >> 1; // Arthur en big endian
            }
            out[i][j] = val;
        }
    }
}

void threshold_compact_bigend(uint8_t** in, uint8_t** out, int w, int h, uint8_t threshold) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            uint8_t val = 0;
            uint8_t mask = 0x80;

            // boucle sur les 8 bits
            for (int k = 0; k < 8; k++) {
                val |= (in[i][(j << 3) + k] < threshold) ? 0 : mask;
                mask = mask >> 1;
            }
            out[i][j] = val;
        }
    }
}

void threshold_norme_littleend(float** U, float** V, uint8_t** out, int i0, int i1, int j0, int j1, float threshold) {
    float t2 = threshold * threshold; // comparaison avec le carre de la norme
    uint8_t val = 0;

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {

            float u = U[i][j];
            float v = V[i][j];
            float norme2 = u * u + v * v; // norme au carre
            val = (norme2 < t2) ? 0 : 255;

            out[i][j] = val;
        }
    }
}

void threshold_norme_compact_littleend(float** U, float** V, uint8_t** out, int w, int h, float threshold) {
    float t2 = threshold * threshold; // comparaison avec le carre de la norme

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            uint8_t val = 0;
            uint8_t mask = 0x1; // little endian

            // boucle sur les 8 bits
            for (int k = 0; k < 8; k++) {

                float u = U[i][(j << 3) + k];
                float v = V[i][(j << 3) + k];
                float norme2 = u * u + v * v; // norme au carre
                val |= (norme2 < t2) ? 0 : mask;
                mask = mask << 1; // little endian
            }
            out[i][j] = val;
        }
    }
}

void pack_ui8vector_bigend(uint8_t* X1, int src_width, uint8_t* Y8) {
    int dst_width = src_width / 8;

    for (int i = 0; i < dst_width; i++) {

        uint8_t x = 0;
        for (int k = 0; k < 8; k++) {

            uint8_t b = X1[8 * i + k];
            x = (x << 1) | b;
        }
        Y8[i] = x;
    }
}

void pack_ui8matrix_bigend(uint8_t** X1, uint8_t** Y8, int src_width, int src_height) {
    for (int i = 0; i < src_height; i++) {
        pack_ui8vector_bigend(X1[i], src_width, Y8[i]);
    }
}

void pack_ui8vector_littleend(uint8_t* X1, int src_width, uint8_t* Y8) {

    int dst_width = src_width / 8;

    for (int i = 0; i < dst_width; i++) {

        uint8_t x = 0;
        for (int k = 0; k < 8; k++) {

            uint8_t b = X1[8 * i + k];
            x = x | (b << k);
        }
        Y8[i] = x;
    }
}

void pack_ui8matrix_littleend(uint8_t** X1, int src_height, int src_width, uint8_t** Y8) {
    for (int i = 0; i < src_height; i++) {
        pack_ui8vector_littleend(X1[i], src_width, Y8[i]);
    }
}

void pack255_ui8vector_littleend(uint8_t* X1, int src_width, uint8_t* Y8) {

    int dst_width = src_width / 8;

    for (int i = 0; i < dst_width; i++) {

        uint8_t x = 0;

        for (int k = 0; k < 8; k++) {

            uint8_t b = (X1[8 * i + k] == 255);
            x = x | (b << k);
        }
        Y8[i] = x;
    }
}

void pack255_ui8matrix_littleend(uint8_t** X1, int src_height, int src_width, uint8_t** Y8) {
    for (int i = 0; i < src_height; i++) {
        pack255_ui8vector_littleend(X1[i], src_width, Y8[i]);
    }
}

void unpack_ui8matrix_bigend(uint8_t** in, uint8_t** out, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            uint8_t mask = 0x80;
            for (int k = 0; k < 8; k++) {
                out[i][(j << 3) + k] = ((in[i][j] & mask) == 0) ? 0 : 1;
                mask = mask >> 1;
            }
        }
    }
}

void unpack_ui32matrix_bigend(uint8_t** in, uint32_t** out, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            uint8_t mask = 0x80;
            for (int k = 0; k < 8; k++) {
                out[i][(j << 3) + k] = ((in[i][j] & mask) == 0) ? 0 : 255;
                mask = mask >> 1;
            }
        }
    }
}

void unpack255_ui8matrix_littleend(uint8_t** in, uint8_t** out, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            uint8_t mask = 0x01;
            for (int k = 0; k < 8; k++) {
                out[i][(j << 3) + k] = ((in[i][j] & mask) == 0) ? 0 : 255;
                mask = mask << 1;
            }
        }
    }
}

void unpack_ui8vector_littleend(uint8_t* X8, int src_width, uint8_t* Y1) {
    for (int i = 0; i < src_width; i++) {

        uint8_t x = X8[i];

        for (int k = 0; k < 8; k++) {
            Y1[8 * i + k] = x & 1;
            x = x >> 1;
        }
    }
}

void unpack_ui8matrix_littleend(uint8_t** X8, int src_height, int src_width, uint8_t** Y1) {
    for (int i = 0; i < src_height; i++) {
        unpack_ui8vector_littleend(X8[i], src_width, Y1[i]);
    }
}

void unpack_ui32matrix_littlend(uint8_t** in, uint32_t** out, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            uint8_t mask = 0x01;
            for (int k = 0; k < 8; k++) {
                out[i][(j << 3) + k] = ((in[i][j] & mask) == 0) ? 0 : 255;
                mask = mask << 1;
            }
        }
    }
}

void mask_EDT(uint8_t** I1, uint8_t** I2, int i0, int i1, int j0, int j1) {
    int i, j;
    uint8_t a, b;
    for (i = i0; i <= i1; i++) {
        for (j = j0; j <= j1; j++) {
            a = I1[i][j];
            b = I2[i][j];
            if (a == 0 && b != 0) {
                I2[i][j] = 0;
            }
        }
    }
}

void mask_EDT_ui32matrix(uint32_t** I1, uint32_t** I2, int i0, int i1, int j0, int j1) {
    int i, j;
    uint32_t a, b;
    for (i = i0; i <= i1; i++) {
        for (j = j0; j <= j1; j++) {
            a = I1[i][j];
            b = I2[i][j];
            if (a == 0 && b != 0) {
                I2[i][j] = 0;
            }
        }
    }
}

void mask_EDT_compact(uint8_t** I1, uint8_t** I2, int i0, int i1, int j0, int j1) {
    int i, j, k;
    uint8_t a, b, mask, bit;
    for (i = i0; i <= i1; i++) {
        for (j = j0; j <= j1; j++) {
            mask = 0x1;
            a = I1[i][j];
            for (k = 0; k < 8; k++) {
                bit = (a & mask) >> k;
                b = I2[i][(j << 3) + k];
                if (bit == 0 && b != 0) {
                    I2[i][(j << 3) + k] = 0;
                }
                mask = mask << 1;
            }
        }
    }
}

void histogram(uint8_t** m, float* h, int i0, int i1, int j0, int j1) {
    /* Histogramme normalisé h[i] = n[i] / N */
    float N = (float)((i1 - i0 + 1) * (j1 - j0 + 1));

    for (int i = 0; i < GRAY_LEVEL; i++) {
        h[i] = 0.0;
    }

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            h[m[i][j]] += 1.0;
        }
    }

    for (int i = 0; i < GRAY_LEVEL; i++) {
        h[i] /= N;
    }
}

void histogram_uv_norm_sq(uint8_t** m, float** U, float** V, float* h, int i0, int i1, int j0, int j1) {
    for (int i = 0; i < GRAY_LEVEL; i++) {
        h[i] = 0.0;
    }

    float N = 0.0;
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            float val = U[i][j] * U[i][j] + V[i][j] * V[i][j];
            h[m[i][j]] += val;
            N += val;
        }
    }

    for (int i = 0; i < GRAY_LEVEL; i++) {
        h[i] /= N;
    }
}

/* Otsu's thresholding method */
/* Between class variance     */
int otsu_bcv(float* h, int t0, int t1) {
    // Init class weights and means (u = sum(i * p(i)) / w)
    float w0 = 0.0;
    float sum0 = 0.0;
    for (int i = 0; i <= t0; i++) {
        w0 += h[i];
        sum0 += t0 * h[i]; // Toutes les intensités < t0 sont mises à t0
    }
    float u0 = (w0 > 0) ? sum0 / w0 : 0.0;

    float w1 = 1.0 - w0;
    float sum1 = 0.0;
    for (int i = t0 + 1; i < GRAY_LEVEL; i++) {
        sum1 += i * h[i];
    }
    float u1 = (w1 > 0) ? sum1 / w1 : 0.0;

    float max_bcv = w0 * w1 * (u0 - u1) * (u0 - u1);
    int threshold = t0;

    // Find max bcv between t0 and t1
    float p, bcv;
    for (int t = t0 + 1; t <= t1; t++) {
        p = h[t];

        w0 += p;
        w1 -= p;

        sum0 += t * p;
        sum1 -= t * p;

        u0 = (w0 > 0) ? sum0 / w0 : 0.0;
        u1 = (w1 > 0) ? sum1 / w1 : 0.0;

        bcv = w0 * w1 * (u0 - u1) * (u0 - u1);

        if (max_bcv < bcv) {
            max_bcv = bcv;
            threshold = t;
        }
    }

    return threshold;
}

int otsu_bcv_k(float* h, int k) {
    int t = 0;
    for (int i = 0; i < k; i++) {
        t = otsu_bcv(h, t, GRAY_LEVEL - 1);
    }
    return t;
}

/* Otsu's thresholding method */
/* Within class variance */
/*int otsu_wcv(uint8_t** m, int i0, int i1, int j0, int j1) {
    int i, j;
    int histVal;
    int np; // Number of pixels of provided image
    int sum = 0, sumBg = 0, sumFg = 0; // Sum of all spikes, sum of background spikes, sum of foreground spikes
    int wBgNum = 0, wFgNum = 0; // Background weight, foreground weight
    float meanBg, meanFg;
    float sigBgNum, sigFgNum;
    float sigBg, sigFg;
    float wcv; // Within Class Variance
    int h[GRAY_LEVEL];
    float value = -1;
    int threshold = 1;

    histogram(m, h, i0, i1, j0, j1);

    // Calculation
    for(i = 0; i < GRAY_LEVEL; i++){ // Sum
        sum += i * h[i];
    }

    np = (i1-i0+1)*(j1-j0+1);

    for(i = 0; i < GRAY_LEVEL; i++){
        histVal = h[i - 1];
        if(!histVal){
            histVal = 0;
        }

        sigBgNum = 0;
        sigFgNum = 0;

        wBgNum += histVal;
        wFgNum = np - wBgNum;

        if(!wFgNum){ // If no pixel above that greyscale, break
            break;
        }

        sumBg += (i - 1) * histVal;
        sumFg = sum - sumBg;

        meanBg = wBgNum ? ((float) sumBg / wBgNum) : 0.0;
        meanFg = wFgNum ? ((float) sumFg / wFgNum) : 0.0;

        // wcv specific
        for (j=0 ; j<i ; j++) { // BG sigma numerator
            sigBgNum += (j - meanBg) * (j - meanBg) * h[j];
        }

        for (j=i ; j<GRAY_LEVEL ; j++) { // FG sigma numerator
            sigFgNum += (j - meanFg) * (j - meanFg) * h[j];
        }

        if (sumBg) {
            sigBg = sigBgNum / wBgNum;
        } else {
            sigBg = 0;
        }

        if (sumFg) {
            sigFg = sigFgNum / wFgNum;
        } else {
            sigFg = 0;
        }

        // Within Class Variance
        wcv = ((float) wBgNum / np) * sigBg + ((float) wFgNum / np) * sigFg;


        // First value assignment
        if (value == -1) {
            value = wcv;
        }

        // Checking if lower value
        if (wcv < value) {
            value = wcv;
            threshold = i;
        }
    }

    return threshold;
}*/

// OpenMP
void unpack_ui8matrix_omp(uint8_t** in, uint8_t** out, int w, int h) {
#ifdef _OPENMP
#pragma omp parallel for shared(in, out)
#endif
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            uint8_t mask = 0x80;
            for (int k = 0; k < 8; k++) {
                out[i][(j << 3) + k] = ((in[i][j] & mask) == 0) ? 0 : 255;
                mask = mask >> 1;
            }
        }
    }
}

void unpack_ui32matrix_omp(uint8_t** in, uint32_t** out, int w, int h) {
#ifdef _OPENMP
#pragma omp parallel for shared(in, out)
#endif
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            uint8_t mask = 0x80;
            for (int k = 0; k < 8; k++) {
                out[i][(j << 3) + k] = ((in[i][j] & mask) == 0) ? 0 : 255;
                mask = mask >> 1;
            }
        }
    }
}
