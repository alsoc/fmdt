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

#include "fmdt/macros.h"
#include "vec.h"

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

void frame_draw_boxes(frame_t* frame, const framebuffer_data_t* fb, const vec_track_t tracks) {
    const int border = 2;
    const int delta_bb = 5;

    const size_t n_tracks = vector_size(tracks);
    for (size_t i = 0; i < n_tracks; i++) {
        const uint32_t track_id = tracks[i].id;
        if (track_id && (tracks[i].end.frame >= frame->id && tracks[i].begin.frame <= frame->id)) {
            const rgb8_t color = image_get_color(g_obj_to_color[tracks[i].obj_type]);

            const size_t offset = (tracks[i].end.frame - frame->id);
            assert(tracks[i].RoIs_id != NULL);
            const size_t RoIs_id_size = vector_size(tracks[i].RoIs_id);
            assert(RoIs_id_size > offset);
            const uint32_t RoI_id = tracks[i].RoIs_id[(RoIs_id_size - 1) - offset];

            RoI_basic_t *RoIs_tmp = frame->RoIs;
            if (RoI_id) {
                const uint32_t track_x = (uint32_t)roundf(RoIs_tmp[RoI_id -1].x);
                const uint32_t track_y = (uint32_t)roundf(RoIs_tmp[RoI_id -1].y);
                const uint32_t track_rx = (RoIs_tmp[RoI_id -1].xmax - RoIs_tmp[RoI_id -1].xmin) / 2;
                const uint32_t track_ry = (RoIs_tmp[RoI_id -1].ymax - RoIs_tmp[RoI_id -1].ymin) / 2;

                const int ymin = CLAMP(track_y - (track_ry + delta_bb), border + 1, fb->frame_height - (border + 2));
                const int ymax = CLAMP(track_y + (track_ry + delta_bb), border + 1, fb->frame_height - (border + 2));
                const int xmin = CLAMP(track_x - (track_rx + delta_bb), border + 1, fb->frame_width  - (border + 2));
                const int xmax = CLAMP(track_x + (track_rx + delta_bb), border + 1, fb->frame_width  - (border + 2));

                image_color_draw_bounding_box(frame->img, ymin, ymax, xmin, xmax, border, color, 0);
           }
        }
    }
}

void frame_write(frame_t* frame, video_writer_t* video_writer) {
    assert(frame != NULL && video_writer != NULL);
    video_writer_save_frame(video_writer, (const uint8_t**)image_color_get_pixels_2d(frame->img));
}