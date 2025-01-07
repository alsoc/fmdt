#include <stdlib.h>
#include <assert.h>

#include "fmdt/framebuffer/framebuffer_io.h"
#include "fmdt/image/image_compute.h"

framebuffer_data_t* framebuffer_alloc_init(const size_t size, const size_t frame_height, const size_t frame_width,
                                           const enum pixfmt_e frame_pixfmt) {
    assert(size > 0);
    framebuffer_data_t* fb = (framebuffer_data_t*)malloc(sizeof(framebuffer_data_t));

    fb->size          = size;
    fb->id_read       = 0;
    fb->id_write      = 0;
    fb->n_filled      = 0;
    fb->frame_height  = frame_height;
    fb->frame_width   = frame_width;
    fb->frames        = (img_data_t**)malloc(sizeof(img_data_t*)*size);
    for (size_t i = 0; i < fb->size; i++) {
        fb->frames[i] = image_color_alloc(frame_height, frame_width);
    }
    fb->frame_pixfmt  = frame_pixfmt;

    return fb;
}

void framebuffer_free(framebuffer_data_t* fb) {
    for (size_t i = 0; i < fb->size; i++) {
        image_color_free(fb->frames[i]);
    }
    free(fb);
}
