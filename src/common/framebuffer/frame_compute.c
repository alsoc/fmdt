#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

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

    for (int i = 0; i < N_OBJECTS; i++) {
        const int ymin = (i + 1) * v_space + (i + 0) * box_size;
        const int ymax = (i + 1) * v_space + (i + 1) * box_size;
        const int xmin = (+1)    * h_space + (+0)    * box_size;
        const int xmax = (+1)    * h_space + (+1)    * box_size;

        const unsigned x = 2 * h_space + box_size;
        const unsigned y = ((i + 1) * v_space + (i + 1) * box_size) - 2;

        const rgb8_t color = image_get_color(g_obj_to_color[i]);

        image_color_draw_bounding_box(frame->img, ymin, ymax, xmin, xmax, border, color, is_dashed);
        image_color_draw_text(frame->img, g_obj_to_string[i], color, y, x);
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

        char str_fpmeteor[] = "fp meteor";
        image_color_draw_text(frame->img, str_fpmeteor, color, y, x);
    }
#endif
}

void frame_draw_boxes(frame_t* frame, const framebuffer_data_t* fb, const vec_track_t tracks, const int draw_id) {
    const int border = 2;
    const int delta_bb = 5;

    const size_t n_tracks = vector_size(tracks);
    for (size_t i = 0; i < n_tracks; i++) {
        const uint32_t track_id = tracks[i].id;
        if (track_id && (tracks[i].end.frame >= frame->id && tracks[i].begin.frame <= frame->id)) {
            const rgb8_t color = image_get_color(g_obj_to_color[tracks[i].obj_type]);

            const size_t offset = (tracks[i].end.frame - frame->id)  / (fb->frame_skip + 1);
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

                if (draw_id) {
                    const int pos_x = xmax + 3;
                    const int pos_y = (ymin) + ((ymax - ymin) / 2);

                    char utostr[11];
                    snprintf(utostr, 11, "%u", track_id);

                    image_color_draw_text(frame->img, utostr, color, pos_y, pos_x);
                }
           }
        }
    }
}

void frame_write(frame_t* frame, video_writer_t* video_writer) {
    assert(frame != NULL && video_writer != NULL);
    video_writer_save_frame(video_writer, (const uint8_t**)image_color_get_pixels_2d(frame->img));
}

frame_extractor_t* frame_extractor_alloc_init(const char* path_begin, const char* path_end, const size_t n_writers,
                                              const size_t frame_height, const size_t frame_width,
                                              const size_t n_threads, const enum pixfmt_e frame_pixfmt,
                                              const enum video_codec_e codec)
{
    frame_extractor_t* fe = (frame_extractor_t*)malloc(sizeof(frame_extractor_t));
    fe->n_writers  = n_writers;
    fe->track_ids  = (size_t*)malloc(sizeof(size_t)*n_writers);
    fe->writers    = (video_writer_t**)malloc(sizeof(video_writer_t*)*n_writers);

    for(size_t i=0; i<n_writers; i++) {
        fe->track_ids[i] = 0;
        fe->writers[i]   = NULL;
    }

    fe->path_begin = strdup(path_begin);
    fe->path_end   = strdup(path_end);
    fe->path_len  = strlen(path_begin) + strlen(path_end) + 50;

    fe->n_threads    = n_threads;
    fe->frame_height = frame_height;
    fe->frame_width  = frame_width;
    fe->frame_pixfmt = frame_pixfmt;
    fe->codec_type   = codec;

    return fe;
}


static inline int _find_frame_extractor(frame_extractor_t* frame_extractor, size_t track_id) {
    for(size_t i=0; i<frame_extractor->n_writers;i++)
        if (frame_extractor->track_ids[i] == track_id)
            return i;
    return -1;
}


void frame_extract(frame_t* frame, frame_extractor_t* frame_extractor, const vec_track_t tracks) {
    const size_t n_tracks = vector_size(tracks);
    for (size_t i = 0; i < n_tracks; i++) {
        const uint32_t track_id = tracks[i].id;

        if (track_id && tracks[i].obj_type == OBJ_METEOR && (tracks[i].end.frame >= frame->id && tracks[i].begin.frame <= frame->id)) {

            if (frame->id == tracks[i].begin.frame) { // new meteor

                // find an available writer or ignore
                int fe_id = _find_frame_extractor(frame_extractor, 0);
                if(fe_id < 0)
                    continue;

                char path_begin[frame_extractor->path_len];
                snprintf(path_begin, frame_extractor->path_len, frame_extractor->path_begin, track_id);
                char path[frame_extractor->path_len];
                snprintf(path, frame_extractor->path_len, "%s%s", path_begin, frame_extractor->path_end);

                frame_extractor->track_ids[fe_id] = track_id;
                frame_extractor->writers[fe_id] = video_writer_alloc_init(path, 0, frame_extractor->n_threads,
                                                                          frame_extractor->frame_height,
                                                                          frame_extractor->frame_width,
                                                                          frame_extractor->frame_pixfmt,
                                                                          frame_extractor->codec_type, 0, 0, 0);

                // write the frame
                video_writer_save_frame(frame_extractor->writers[fe_id], (const uint8_t**)image_color_get_pixels_2d(frame->img));
            } else {
                // find the allocated writer or ignore
                int fe_id = _find_frame_extractor(frame_extractor, track_id);
                if(fe_id < 0)
                    continue;

                // write the frame
                video_writer_save_frame(frame_extractor->writers[fe_id], (const uint8_t**)image_color_get_pixels_2d(frame->img));


                // Last frame of the track : free the writter
                if (frame->id == tracks[i].end.frame) {
                    video_writer_free(frame_extractor->writers[fe_id]);
                    frame_extractor->writers[fe_id] = NULL;
                    frame_extractor->track_ids[fe_id] = 0;
                }
            }
        }

    }
}

void frame_extractor_free(frame_extractor_t* frame_extractor)
{
    free(frame_extractor->path_begin);
    free(frame_extractor->path_end);

    for(size_t i=0; i<frame_extractor->n_writers; i++)
        if(frame_extractor->writers[i])
            video_writer_free(frame_extractor->writers[i]);

    free(frame_extractor->track_ids);
    free(frame_extractor->writers);

    free(frame_extractor);
}