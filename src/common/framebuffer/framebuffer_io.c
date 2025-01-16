#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "fmdt/framebuffer/framebuffer_io.h"
#include "fmdt/image/image_compute.h"
#include "fmdt/features/features_compute.h"
#include "vec.h"


framebuffer_data_t* framebuffer_alloc_init(const size_t size, const size_t frame_height, const size_t frame_width,
                                           const size_t frame_skip, const enum pixfmt_e frame_pixfmt,
                                           const size_t max_RoIs_size) {
    assert(size > 0);
    framebuffer_data_t* fb = (framebuffer_data_t*)malloc(sizeof(framebuffer_data_t));

    fb->size          = size;
    fb->id_read       = 0;
    fb->id_write      = 0;
    fb->n_filled      = 0;
    fb->frame_height  = frame_height;
    fb->frame_width   = frame_width;
    fb->frame_skip    = frame_skip;
    fb->max_RoIs_size = max_RoIs_size;
    fb->frames        = (frame_t*)malloc(sizeof(frame_t)*size);
    fb->actions       = NULL;

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

static inline frame_t* _pop(framebuffer_data_t* fb) {
    fb->n_filled--;
    frame_t* frame = &fb->frames[(fb->id_read++) % fb->size];

    framebuffer_action_t* action = fb->actions;
    while(action) {
        action->apply(frame, action->args);
        action = action->next;
    }

    return frame;
}

frame_t* framebuffer_pop(framebuffer_data_t* fb) {
    return (fb->n_filled == fb->size)?_pop(fb):NULL;
}

void framebuffer_flush(framebuffer_data_t* fb) {
    while(fb->n_filled > 0)
        _pop(fb);
}

framebuffer_action* framebuffer_action_alloc(size_t n_args) {
    return (framebuffer_action*)malloc(sizeof(framebuffer_action) + n_args * sizeof(void*));
}

void framebuffer_action_register(framebuffer_data_t* fb, framebuffer_action* action) {
    assert(fb != NULL && action != NULL);
    action->next = fb->actions;
    fb->actions = action;
}

void framebuffer_action_free(framebuffer_action* action) {
    assert(action != NULL);
    if(action->free != NULL)
        action->free(action->args);
    free(action);
}

void framebuffer_free(framebuffer_data_t* fb) {
    for (size_t i = 0; i < fb->size; i++) {
        image_color_free(fb->frames[i].img);
        features_free_RoIs_basic(fb->frames[i].RoIs);
    }
    free(fb->frames);

    framebuffer_action_t* action;
    while( (action = fb->actions) ) {
        fb->actions = action->next;
        framebuffer_action_free(action);
    }

    free(fb);
}
