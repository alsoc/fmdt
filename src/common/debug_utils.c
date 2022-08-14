/**
 * Copyright (c) 2017-2018, Arthur Hennequin, LIP6, UPMC, CNRS
 * Copyright (c) 2020-2020, Lionel Lacassagne, LIP6 Sorbonne University, CNRS
 * Copyright (c) 2020-2021, MILLET Maxime, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <ffmpeg-io/reader.h>
#include <ffmpeg-io/writer.h>
#include <nrc2.h>

#include "args.h"
#include "CCL.h"
#include "debug_utils.h"
#include "features.h"
#include "threshold.h"
#include "tracking.h"
#include "video.h"
#include "macro_debug.h"
#include "tools_visu.h"

#define SIZE_BUF 20

void save_bounding_box(const char* filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame) {
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }
    fprintf(f, "%4d \t %4d \t %4d \t %4d \t %4d \n", frame, rx, ry, bb_x, bb_y);
    fclose(f);
}

void plot_bounding_box(rgb8** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8 color) {
    for (int b = 0; b < border; b++) {
        ymin++;
        ymax--;
        xmin++;
        xmax--;
        for (int i = ymin; i <= ymax; i++) {
            img[i][xmin] = color;
            img[i][xmax] = color;
        }
        for (int j = xmin; j <= xmax; j++) {
            img[ymin][j] = color;
            img[ymax][j] = color;
        }
    }
}

void filter_speed_binarize(uint32** in, int i0, int i1, int j0, int j1, uint8** out, ROI_t* stats) {
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (stats[in[i][j]].S != 0) {
                out[i][j] = 0xFF;
            } else {
                out[i][j] = 0;
            }
        }
    }
}

rgb8** load_image_color(const char* filename, long* i0, long* i1, long* j0, long* j1) {
    VERBOSE(printf("%s\n", filename););
    ffmpeg_handle reader;
    ffmpeg_init(&reader);
    ffmpeg_options options;
    ffmpeg_options_init(&options);

    options.debug = 1;

    if (!ffmpeg_probe(&reader, filename, &options)) {
        fprintf(stderr, "(EE) Error in load_image_color: %s (%d)\n", ffmpeg_error2str(reader.error), reader.error);
        exit(0);
    }
    reader.output.pixfmt = ffmpeg_str2pixfmt("rgb24");
    *i0 = 0;
    *j0 = 0;
    *i1 = reader.input.height - 1;
    *j1 = reader.input.width - 1;
    VERBOSE(printf("i1=%ld j1=%ld\n", *i1, *j1););
    rgb8** img = rgb8matrix(*i0, *i1, *j0, *j1);

    ffmpeg_start_reader(&reader, filename, NULL);
    ffmpeg_read2d(&reader, (uint8_t**)img);
    ffmpeg_stop_reader(&reader);
    return img;
}

void save_frame_threshold(const char* filename, uint8** I0, uint8** I1, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, h - 1, 0, 2 * w - 1);
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

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        write_PNM_row((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h - 1, 0, 2 * w - 1);
}

void save_frame_ui32matrix(const char* filename, uint32** I, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, h - 1, 0, w - 1);
    if (img == NULL)
        return;

    // (1,1) : filtrage surface
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = (I[i][j] == 0) ? 0 : 255;
            img[i][j].g = (I[i][j] == 0) ? 0 : 255;
            img[i][j].b = (I[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        write_PNM_row((uint8*)img[i], w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h - 1, 0, w - 1);
}

void save_frame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0,
                         int j1) {
    rgb8 green;
    green.g = 255;
    green.b = 000;
    green.r = 000;

    rgb8 red;
    red.g = 000;
    red.b = 000;
    red.r = 255;

    rgb8 blue;
    blue.g = 000;
    blue.b = 255;
    blue.r = 000;

    rgb8 orange;
    orange.r = 255;
    orange.g = 165;
    orange.b = 000;

    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, h - 1, 0, w - 1);
    if (img == NULL)
        return;

    // (1,1) : filtrage surface
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    for (int i = 0; i < tracks_nb + 1; i++)
        if (tracks[i].state != TRACK_LOST && tracks[i].time >= 2 && tracks[i].state != TRACK_EXTRAPOLATED &&
            tracks[i].state != TRACK_FINISHED) {
            int ymin = clamp(tracks[i].bb_y - tracks[i].ry, 1, i1 - 1);
            int ymax = clamp(tracks[i].bb_y + tracks[i].ry, 1, i1 - 1);
            int xmin = clamp(tracks[i].bb_x - tracks[i].rx, 1, j1 - 1);
            int xmax = clamp(tracks[i].bb_x + tracks[i].rx, 1, j1 - 1);
            if (tracks[i].time >= 2)
                plot_bounding_box(img, ymin, ymax, xmin, xmax, 1, green);
        }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        write_PNM_row((uint8*)img[i], w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h - 1, 0, w - 1);
}

void save_video_frame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0,
                               int j1) {
    rgb8 green;
    green.g = 255;
    green.b = 000;
    green.r = 000;

    rgb8 red;
    red.g = 000;
    red.b = 000;
    red.r = 255;

    rgb8 blue;
    blue.g = 000;
    blue.b = 255;
    blue.r = 000;

    rgb8 orange;
    orange.r = 255;
    orange.g = 165;
    orange.b = 000;

    static ffmpeg_handle writer;
    if (writer.pipe == NULL) {
        ffmpeg_init(&writer);
        writer.input.width = j1 - j0 + 1;
        writer.input.height = i1 - i0 + 1;
        writer.input.pixfmt = ffmpeg_str2pixfmt("rgb24");
        if (!ffmpeg_start_writer(&writer, filename, NULL))
            return;
    }
    rgb8** img = rgb8matrix(0, i1, 0, j1);
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    for (int i = 0; i < tracks_nb + 1; i++)
        if (tracks[i].state != TRACK_LOST && tracks[i].time >= 3 && tracks[i].state != TRACK_EXTRAPOLATED &&
            tracks[i].state != TRACK_FINISHED) {
            int ymin = clamp(tracks[i].bb_y - tracks[i].ry - 5, 1, i1 - 1);
            int ymax = clamp(tracks[i].bb_y + tracks[i].ry + 5, 1, i1 - 1);
            int xmin = clamp(tracks[i].bb_x - tracks[i].rx - 5, 1, j1 - 1);
            int xmax = clamp(tracks[i].bb_x + tracks[i].rx + 5, 1, j1 - 1);
            if (tracks[i].time >= 3)
                plot_bounding_box(img, ymin, ymax, xmin, xmax, 2, green);
        }

    ffmpeg_write2d(&writer, (uint8_t**)img);
    free_rgb8matrix(img, 0, i1, 0, j1);
}

void save_frame_ui8matrix(const char* filename, uint8** I, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, h - 1, 0, w - 1);
    if (img == NULL)
        return;

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */
    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        write_PNM_row((uint8*)img[i], w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h - 1, 0, w - 1);
}

void HsvToRgb(rgb8* pixel, uint8 h, uint8 s, uint8 v) {
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

void save_frame_quad(const char* filename, uint8** I0, uint8** I1, uint32** I2, uint32** I3, int nbLabel,
                     ROI_t* stats, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, 2 * h - 1, 0, 2 * w - 1);
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
                HsvToRgb((img[i + h]) + j, (uint8)(I2[i][j] * 360 / 5), 255, 255);
            } else {
                HsvToRgb((img[i + h]) + j, 0, 0, 0);
            }
        }
    }
    // Carrés vert sur les CC
    // for (int k=1 ; k<=nbLabel ; k++) {
    //     if (stats[k].S > 3) {
    //         rgb8 color;
    //         //HsvToRgb(&color, k*255 / nbLabel, 255, 255);
    //         color.r = 0;
    //         color.g = 255;
    //         color.b = 0;
    //         int ymin = min(stats[k].ymin + h - 10, h*2-1);
    //         int ymax = min(stats[k].ymax + h + 10, h*2-1);
    //         int xmin = min(stats[k].xmin     - 10, w  -1);
    //         int xmax = min(stats[k].xmax     + 10, w  -1);

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

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= 2 * h - 1; i++)
        write_PNM_row((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, 2 * h - 1, 0, 2 * w - 1);
}

void save_frame_quad_hysteresis(const char* filename, uint8** I0, uint32** SH, uint32** SB, uint32** Y, int i0, int i1,
                                int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, 2 * h - 1, 0, 2 * w - 1);
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
                HsvToRgb((img[i]) + j + w, (uint8)(SB[i][j] * 360 / 5), 255, 255);
            } else {
                HsvToRgb((img[i]) + j + w, 0, 0, 0);
            }
        }
    }

    // (0,1) : seuillage high
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (SH[i][j] != 0) {
                HsvToRgb((img[i + h]) + j, (uint8)(SH[i][j] * 360 / 5), 255, 255);
            } else {
                HsvToRgb((img[i + h]) + j, 0, 0, 0);
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

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= 2 * h - 1; i++)
        write_PNM_row((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, 2 * h - 1, 0, 2 * w - 1);
}

void save_max(const char* filename, uint8** I, int i0, int i1, int j0, int j1) {
    uint8 m;
    uint8* res = ui8vector(i0, i1);

    zero_ui8vector(res, i0, i1);

    for (int i = i0; i <= i1; i++) {
        m = 0;
        for (int j = j0; j <= j1; j++) {
            m = max(m, I[i][j]);
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

void create_folder(char* folder_path) {
    struct stat status = {0};
    if (stat(folder_path, &status) == -1)
        mkdir(folder_path, 0700);
}

void copy_ui8matrix_ui8matrix(uint8** X, int i0, int i1, int j0, int j1, uint8** Y) {
    for (int i = i0; i <= i1; i++)
        for (int j = j0; j <= j1; j++)
            Y[i][j] = X[i][j];
}

void convert_ui8vector_ui32vector(uint8* X, long nl, long nh, uint32* Y) {
    for (long i = nl; i <= nh; i++)
        Y[i] = (uint32)X[i];
}

void convert_ui8matrix_ui32matrix(uint8** X, int nrl, int nrh, int ncl, int nch, uint32** Y) {
    for (long i = nrl; i <= nrh; i++)
        convert_ui8vector_ui32vector(X[i], ncl, nch, Y[i]);
}

void write_PNM_row(uint8* line, int width, FILE* file) {
    /* Le fichier est deja ouvert et ne sera pas ferme a la fin */
    fwrite(&(line[0]), sizeof(byte), 3 * sizeof(byte) * width, file);
}
