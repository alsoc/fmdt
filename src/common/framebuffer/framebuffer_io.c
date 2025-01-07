#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "fmdt/framebuffer/framebuffer_io.h"
#include "fmdt/image/image_compute.h"
#include "fmdt/video/video_io.h"

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

void framebuffer_bufferize(framebuffer_data_t* fb, const uint8_t** img_rgb24) {
    size_t pixsize = image_get_pixsize(fb->frame_pixfmt);
    for (size_t i = 0; i < fb->frame_height; i++)
        memcpy(image_color_get_pixels_2d(fb->frames[fb->id_write % fb->size])[i], img_rgb24[i], fb->frame_width * sizeof(uint8_t) * pixsize);
    fb->id_write++;
}

void framebuffer_draw_frame_id(framebuffer_data_t* fb, int frame_id) {
#ifdef FMDT_OPENCV_LINK
    image_color_draw_frame_id( fb->frames[(fb->id_write-1) % fb->size] , frame_id);
#endif
}

void framebuffer_save(framebuffer_data_t* fb, video_writer_t* video_writer) {
    assert(fb->id_write > 0);

    video_writer_save_frame(video_writer, (const uint8_t**)image_color_get_pixels_2d(fb->frames[(fb->id_write-1) % fb->size]));
}

void framebuffer_free(framebuffer_data_t* fb) {
    for (size_t i = 0; i < fb->size; i++) {
        image_color_free(fb->frames[i]);
    }
    free(fb);
}
