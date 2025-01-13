#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifdef FMDT_OPENCV_LINK
#include <opencv2/imgproc.hpp>
#endif

#include "fmdt/framebuffer/framebuffer_io.h"
#include "fmdt/image/image_compute.h"
#include "fmdt/video/video_io.h"
#include "fmdt/features/features_compute.h"
#include "fmdt/tracking/tracking_compute.h"
#include "fmdt/tracking/tracking_global.h"


void frame_draw_id(frame_t* frame) {
#ifdef FMDT_OPENCV_LINK
    assert(frame != NULL);
    image_color_draw_frame_id( frame->img , frame->id);
#endif
}

void frame_draw_legend(frame_t* frame, const int validation) {
#ifdef FMDT_OPENCV_LINK
    const unsigned box_size = 20, h_space = 10, v_space = 10, border = 2, is_dashed = 0;
    const float font_size = 0.7f;

    cv::Mat* cv_img = (cv::Mat*)frame->img->pixels;

    for (int i = 0; i < N_OBJECTS; i++) {
        const int ymin = (i + 1) * v_space + (i + 0) * box_size;
        const int ymax = (i + 1) * v_space + (i + 1) * box_size;
        const int xmin = (+1)    * h_space + (+0)    * box_size;
        const int xmax = (+1)    * h_space + (+1)    * box_size;

        const unsigned x = 2 * h_space + box_size;
        const unsigned y = ((i + 1) * v_space + (i + 1) * box_size) - 2;

        const rgb8_t color = image_get_color(g_obj_to_color[i]);

        image_color_draw_bounding_box(frame->img, ymin, ymax, xmin, xmax, border, color, is_dashed);
        cv::putText(*cv_img, std::string(g_obj_to_string[i]), cv::Point(x, y), cv::FONT_HERSHEY_DUPLEX, font_size,
                    cv::Scalar(color.r, color.g, color.b), 1, cv::LINE_AA);
    }

    if (validation) {
        // add false positive meteor
        const int ymin = (N_OBJECTS + 2) * v_space + (N_OBJECTS + 1) * box_size;
        const int ymax = (N_OBJECTS + 2) * v_space + (N_OBJECTS + 2) * box_size;
        const int xmin = (+1)            * h_space + (+0)            * box_size;
        const int xmax = (+1)            * h_space + (+1)            * box_size;

        const unsigned x = 2 * h_space + box_size;
        const unsigned y = ((N_OBJECTS + 2) * v_space + (N_OBJECTS + 2) * box_size) - 2;

        const rgb8_t color = image_get_color(COLOR_RED);

        image_color_draw_bounding_box(frame->img, ymin, ymax, xmin, xmax, border, color, is_dashed);
        cv::putText(*cv_img, std::string("fp meteor"), cv::Point(x, y), cv::FONT_HERSHEY_DUPLEX, font_size,
                    cv::Scalar(color.r, color.g, color.b), 1, cv::LINE_AA);
    }
#endif
}

void frame_draw_boxes(frame_t* frame, const vec_track_t tracks) {

}

void frame_write(frame_t* frame, video_writer_t* video_writer) {
    assert(frame != NULL && video_writer != NULL);
    video_writer_save_frame(video_writer, (const uint8_t**)image_color_get_pixels_2d(frame->img));
}