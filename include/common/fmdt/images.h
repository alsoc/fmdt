#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct {
    int i0, i1, j0, j1;
    size_t frame_start;
    size_t frame_end;
    size_t frame_skip;
    size_t frame_current;
    char** path_files;
    uint8_t*** buffer_files;
    size_t files_count;
} images_t;

images_t* images_init_from_path(const char* path, const size_t start, const size_t end, const size_t skip,
                                const int bufferize);
int images_get_next_frame(images_t* images, uint8_t** I);
void images_free(images_t* images);
