#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "fmdt/framebuffer/framebuffer_io.h"
#include "fmdt/image/image_compute.h"
#include "fmdt/video/video_io.h"
#include "fmdt/features/features_compute.h"

void frame_draw_id(frame_t* frame) {
#ifdef FMDT_OPENCV_LINK
    assert(frame != NULL);
    image_color_draw_frame_id( frame->img , frame->id);
#endif
}

void frame_write(frame_t* frame, video_writer_t* video_writer) {
    assert(frame != NULL && video_writer != NULL);
    video_writer_save_frame(video_writer, (const uint8_t**)image_color_get_pixels_2d(frame->img));
}