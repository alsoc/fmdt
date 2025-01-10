#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "fmdt/framebuffer/framebuffer_io.h"
#include "fmdt/image/image_compute.h"
#include "fmdt/video/video_io.h"
#include "fmdt/features/features_compute.h"

framebuffer_data_t* framebuffer_alloc_init(const size_t size, const size_t frame_height, const size_t frame_width,
                                           const enum pixfmt_e frame_pixfmt, const size_t max_RoIs_size) {
    assert(size > 0);
    framebuffer_data_t* fb = (framebuffer_data_t*)malloc(sizeof(framebuffer_data_t));

    fb->size          = size;
    fb->id_read       = 0;
    fb->id_write      = 0;
    fb->n_filled      = 0;
    fb->frame_height  = frame_height;
    fb->frame_width   = frame_width;
    fb->max_RoIs_size = max_RoIs_size;
    fb->frames        = (frame_t*)malloc(sizeof(frame_t)*size);

    for (size_t i = 0; i < fb->size; i++) {
        fb->frames[i].img    = image_color_alloc(frame_height, frame_width);
        fb->frames[i].RoIs   = features_alloc_RoIs_basic(max_RoIs_size);
        fb->frames[i].n_RoIs = 0;
    }
    fb->frame_pixfmt = frame_pixfmt;

    return fb;
}

void framebuffer_push(framebuffer_data_t* fb, const int frame_id, const uint8_t** img_rgb24, const RoI_basic_t* RoIs,
                      const size_t n_RoIs) {
    size_t buff_id = fb->id_write % fb->size;

    fb->frames[buff_id].id = frame_id;

    // Image Copy
    size_t pixsize = image_get_pixsize(fb->frame_pixfmt);
    for (size_t i = 0; i < fb->frame_height; i++)
        memcpy(image_color_get_pixels_2d(fb->frames[buff_id].img)[i], img_rgb24[i], fb->frame_width * sizeof(uint8_t) * pixsize);

    // Regions Copy
    fb->frames[buff_id].n_RoIs = n_RoIs;
    memcpy(fb->frames[buff_id].RoIs, RoIs, n_RoIs * sizeof(RoI_basic_t));

    // Buffer management
    fb->id_write++;
    if(fb->n_filled < fb->size)
        fb->n_filled++;
}

void framebuffer_draw_frame_id(framebuffer_data_t* fb) {
#ifdef FMDT_OPENCV_LINK
    size_t buff_id = (fb->id_write-1) % fb->size;
    image_color_draw_frame_id( fb->frames[buff_id].img , fb->frames[buff_id].id);
#endif
}

void framebuffer_save(framebuffer_data_t* fb, video_writer_t* video_writer) {
    assert(fb->id_write > 0);

    video_writer_save_frame(video_writer, (const uint8_t**)image_color_get_pixels_2d(fb->frames[(fb->id_write-1) % fb->size].img));
}

void framebuffer_free(framebuffer_data_t* fb) {
    for (size_t i = 0; i < fb->size; i++) {
        image_color_free(fb->frames[i].img);
        features_free_RoIs_basic(fb->frames[i].RoIs);
    }
    free(fb->frames);
    free(fb);
}
