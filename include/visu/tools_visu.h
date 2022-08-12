/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */ 

#ifndef __TOOLS_VISU_H__
#define __TOOLS_VISU_H__

#include <nrc2.h>
#include "Tracking.h"

enum Color_t {MISC = 0, GREEN, RED, PURPLE, ORANGE, BLUE, YELLOW, N_COLOR_T};

#define METEOR_COLOR  GREEN
#define STAR_COLOR    PURPLE
#define NOISE_COLOR   ORANGE
#define UNKNOWN_COLOR MISC

#define METEOR_STR  "meteor"
#define STAR_STR    "star"
#define NOISE_STR   "noise"
#define UNKNOWN_STR "unknown"

extern enum Color_t obj_type_to_color[N_OBJ_TYPES];
extern char obj_type_to_string[N_OBJ_TYPES][64];
extern char obj_type_to_string_with_spaces[N_OBJ_TYPES][64];

typedef struct coordBB {
    int track_id;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    enum Color_t color;
} coordBB;

void init_global_data();
enum Obj_type string_to_obj_type(const char* string);

rgb8 get_color(enum Color_t color);
void convert_img_grayscale_to_rgb(const uint8** I, rgb8** I_bb, int i0, int i1, int j0, int j1);
#ifdef OPENCV_LINK
void draw_track_ids(rgb8** img, const int img_width, const int img_height, const coordBB* listBB, const int nBB);
#endif
void draw_BB(rgb8** I_bb, const coordBB* listBB, int n_BB);
void saveFrame(const char* filename, const rgb8** I_bb, int w, int h);

#endif // __TOOLS_VISU_H__