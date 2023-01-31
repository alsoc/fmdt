#pragma once

#include <stdint.h>

typedef struct {
    size_t max_size;
    uint32_t** nearest;
    float** distances;
    uint32_t* conflicts;
} kNN_data_t;
