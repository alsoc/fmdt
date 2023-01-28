#pragma once

#include <stdint.h>
#include <stdlib.h>

enum color_e { MISC = 0, GRAY, GREEN, RED, PURPLE, ORANGE, BLUE, YELLOW, N_COLORS };
typedef enum color_e* vec_color_e;

typedef struct { uint8_t r; uint8_t g; uint8_t b; } rgb8_t;

typedef struct img_data_t {
    size_t height;
    size_t width;
    void* pixels;
    void* container_2d;
} img_data_t;
