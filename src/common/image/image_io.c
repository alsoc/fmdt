#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nrc2.h>

#include "fmdt/image/image_struct.h"
#include "fmdt/image/image_io.h"

void image_save_frame_threshold(const char* filename, uint8_t** I0, uint8_t** I1, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8_t** img = (rgb8_t**)rgb8matrix(0, h - 1, 0, 2 * w - 1);
    if (img == NULL)
        return;

    // (0,0) : video
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage luminosité
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j + w].r = I1[i][j];
            img[i][j + w].g = I1[i][j];
            img[i][j + w].b = I1[i][j];
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */

    snprintf(buffer, sizeof(buffer), "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        image_write_PNM_row((uint8_t*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix((rgb8**)img, 0, h - 1, 0, 2 * w - 1);
}

void image_HSV_to_RGB(rgb8_t* pixel, uint8_t h, uint8_t s, uint8_t v) {
    unsigned char region, remainder, p, q, t;

    if (s == 0) {
        pixel->r = v;
        pixel->g = v;
        pixel->b = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
    case 0:
        pixel->r = v;
        pixel->g = t;
        pixel->b = p;
        break;
    case 1:
        pixel->r = q;
        pixel->g = v;
        pixel->b = p;
        break;
    case 2:
        pixel->r = p;
        pixel->g = v;
        pixel->b = t;
        break;
    case 3:
        pixel->r = p;
        pixel->g = q;
        pixel->b = v;
        break;
    case 4:
        pixel->r = t;
        pixel->g = p;
        pixel->b = v;
        break;
    default:
        pixel->r = v;
        pixel->g = p;
        pixel->b = q;
        break;
    }
}

void image_save_frame_quad(const char* filename, uint8_t** I0, uint8_t** I1, uint32_t** I2, uint32_t** I3, int nbLabel,
                           RoI_t* stats, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8_t** img = (rgb8_t**)rgb8matrix(0, 2 * h - 1, 0, 2 * w - 1);
    if (img == NULL)
        return;

    // (0,0) : video
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage luminosité
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j + w].r = I1[i][j];
            img[i][j + w].g = I1[i][j];
            img[i][j + w].b = I1[i][j];
        }
    }
    // (1,0) : CCL
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (I2[i][j] != 0) {
                image_HSV_to_RGB((img[i + h]) + j, (uint8_t)(I2[i][j] * 360 / 5), 255, 255);
            } else {
                image_HSV_to_RGB((img[i + h]) + j, 0, 0, 0);
            }
        }
    }
    // Carrés vert sur les CC
    // for (int k=1 ; k<=nbLabel ; k++) {
    //     if (stats[k].S > 3) {
    //         rgb8_t color;
    //         //image_HSV_to_RGB(&color, k*255 / nbLabel, 255, 255);
    //         color.r = 0;
    //         color.g = 255;
    //         color.b = 0;
    //         int ymin = MIN(stats[k].ymin + h - 10, h*2-1);
    //         int ymax = MIN(stats[k].ymax + h + 10, h*2-1);
    //         int xmin = MIN(stats[k].xmin     - 10, w  -1);
    //         int xmax = MIN(stats[k].xmax     + 10, w  -1);

    //         for (int b=0; b<5 ; b++) {
    //             ymin++; ymax--;xmin++;xmax--;
    //             for (int i=ymin ; i<=ymax ; i++) {
    //                 img[i][xmin] = color;
    //                 img[i][xmax] = color;
    //             }
    //             for (int j=xmin ; j<=xmax ; j++) {
    //                 img[ymin][j] = color;
    //                 img[ymax][j] = color;
    //             }
    //         }
    //     }
    // }

    // (1,1) : filtrage surface
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i + h][j + w].r = (I3[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].g = (I3[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].b = (I3[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */

    snprintf(buffer, sizeof(buffer), "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= 2 * h - 1; i++)
        image_write_PNM_row((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix((rgb8**)img, 0, 2 * h - 1, 0, 2 * w - 1);
}

void image_save_frame_quad_hysteresis(const char* filename, uint8_t** I0, uint32_t** SH, uint32_t** SB, uint32_t** Y,
                                      int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8_t** img = (rgb8_t**)rgb8matrix(0, 2 * h - 1, 0, 2 * w - 1);
    if (img == NULL)
        return;

    // (0,0) : video
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage low
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (SB[i][j] != 0) {
                image_HSV_to_RGB((img[i]) + j + w, (uint8_t)(SB[i][j] * 360 / 5), 255, 255);
            } else {
                image_HSV_to_RGB((img[i]) + j + w, 0, 0, 0);
            }
        }
    }

    // (0,1) : seuillage high
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (SH[i][j] != 0) {
                image_HSV_to_RGB((img[i + h]) + j, (uint8_t)(SH[i][j] * 360 / 5), 255, 255);
            } else {
                image_HSV_to_RGB((img[i + h]) + j, 0, 0, 0);
            }
        }
    }

    // (0,0) : out
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i + h][j + w].r = (Y[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].g = (Y[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].b = (Y[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */

    snprintf(buffer, sizeof(buffer), "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= 2 * h - 1; i++)
        image_write_PNM_row((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix((rgb8**)img, 0, 2 * h - 1, 0, 2 * w - 1);
}

void image_write_PNM_row(const uint8_t* line, const int width, FILE* file) {
    /* Le fichier est deja ouvert et ne sera pas ferme a la fin */
    fwrite(&(line[0]), sizeof(byte), 3 * sizeof(byte) * width, file);
}
