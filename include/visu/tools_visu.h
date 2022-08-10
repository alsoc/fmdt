/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */ 

#ifndef _TOOLS_VISU_
#define _TOOLS_VISU_

#include <inttypes.h>
#include <nrc2.h>

#define SIZE_MAX_METEORROI 3000
#define SIZE_MAX_TRACKS 1000
#define TOLERANCE_DISTANCEMIN 20

enum Color_t {MISC = 0, GREEN, RED, ORANGE, BLUE, YELLOW, N_COLOR_T};

typedef struct coordBB {
    int track_id;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    enum Color_t color;
} coordBB;

rgb8 get_color(enum Color_t color);
void convert_img_grayscale_to_rgb(const uint8** I, rgb8** I_bb, int i0, int i1, int j0, int j1);
#ifdef OPENCV_LINK
void draw_track_ids(rgb8** img, const int img_width, const int img_height, const coordBB* listBB, const int nBB);
#endif
void draw_BB(rgb8** I_bb, const coordBB* listBB, int n_BB);
void saveFrame(const char* filename, const rgb8** I_bb, int w, int h);

#endif // _TOOLS_VISU_