/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */

#ifndef __TOOLS_VISU_H__
#define __TOOLS_VISU_H__

#include <nrc2.h>

#include "tracking.h"

typedef struct BB_coord_t {
    int track_id;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    enum color_e color;
} BB_coord_t;

rgb8 tools_get_color(enum color_e color);
void tools_convert_img_grayscale_to_rgb(const uint8** I, rgb8** I_bb, int i0, int i1, int j0, int j1);
#ifdef OPENCV_LINK
void tools_draw_text(rgb8** img, const int img_width, const int img_height, const BB_coord_t* listBB, const int nBB,
                     int validation, int show_ids);
#endif
void tools_draw_BB(rgb8** I_bb, const BB_coord_t* listBB, int n_BB);
void tools_save_frame(const char* filename, const rgb8** I_bb, int w, int h);

#endif // __TOOLS_VISU_H__