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
#include "fmdt/tools.h"
#include "vec.h"

void frame_draw_id(frame_t* frame) {
#ifdef FMDT_OPENCV_LINK
    assert(frame != NULL);
    image_color_draw_frame_id( frame->img , frame->id);
#endif
}

static void _frame_draw_id_action(frame_t* frame, void* args[]) {
    frame_draw_id(frame);
}

void frame_draw_id_action_register(framebuffer_data_t* framebuffer) {
    framebuffer_action_t* action = framebuffer_action_alloc(0);
    action->apply = _frame_draw_id_action;
    action->free = NULL;
    framebuffer_action_register(framebuffer, action);
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

static void _frame_draw_legend_action(frame_t* frame, void* args[]) {
    frame_draw_legend(frame, *(const int*)args[0]);
}

void frame_draw_legend_action_register(framebuffer_data_t* framebuffer, const int* validation) {
    framebuffer_action_t* action = framebuffer_action_alloc(1);
    action->apply   = _frame_draw_legend_action;
    action->free    = NULL;
    action->args[0] = (void*)validation;
    framebuffer_action_register(framebuffer, action);
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

static void _frame_draw_boxes_action(frame_t* frame, void* args[]) {
    frame_draw_boxes(frame, (const framebuffer_data_t*)args[0], ((const tracking_data_t*)args[1])->tracks, *(const int*)args[2]);
}

void frame_draw_boxes_action_register(framebuffer_data_t* framebuffer, const tracking_data_t* tracking_data, const int* draw_id) {
    framebuffer_action_t* action = framebuffer_action_alloc(3);
    action->apply   = _frame_draw_boxes_action;
    action->free    = NULL;
    action->args[0] = (void*)framebuffer;
    action->args[1] = (void*)tracking_data;
    action->args[2] = (void*)draw_id;
    framebuffer_action_register(framebuffer, action);
}

void frame_write(frame_t* frame, video_writer_t* video_writer) {
    assert(frame != NULL && video_writer != NULL);
    video_writer_save_frame(video_writer, (const uint8_t**)image_color_get_pixels_2d(frame->img));
}

static void _frame_write_action(frame_t* frame, void* args[]) {
    frame_write(frame, (video_writer_t*)args[0]);
}

static void _frame_write_action_free(void* args[]) {
    video_writer_free((video_writer_t*)args[0]);
}

void frame_write_action_register(framebuffer_data_t* framebuffer, const char* path, const size_t start,
                                 const size_t n_ffmpeg_threads, const int is_player,
                                 const enum video_codec_e codec_type) {
    video_writer_t* writer = video_writer_alloc_init(path, start, n_ffmpeg_threads, framebuffer->frame_height,
                                                     framebuffer->frame_width, PIXFMT_RGB24, codec_type, is_player, 0, NULL);
    if(!writer) return;

    framebuffer_action_t* action = framebuffer_action_alloc(1);
    action->apply   = _frame_write_action;
    action->free    = _frame_write_action_free;
    action->args[0] = (void*)writer;
    framebuffer_action_register(framebuffer, action);
}

frame_extractor_t* frame_extractor_alloc_init(const char* path, const size_t n_writers, const size_t frame_height,
                                              const size_t frame_width, const size_t n_threads,
                                              const enum pixfmt_e frame_pixfmt, const enum video_codec_e codec)
{
    frame_extractor_t* fe = (frame_extractor_t*)malloc(sizeof(frame_extractor_t));
    fe->n_writers  = n_writers;
    fe->track_ids  = (size_t*)malloc(sizeof(size_t)*n_writers);
    fe->writers    = (video_writer_t**)malloc(sizeof(video_writer_t*)*n_writers);

    for(size_t i=0; i<n_writers; i++) {
        fe->track_ids[i] = 0;
        fe->writers[i]   = NULL;
    }

    fe->path      = strdup(path);
    fe->path_len  = strlen(path) + 128;
    fe->n_meteors = 1;

    fe->n_threads    = n_threads;
    fe->frame_height = frame_height;
    fe->frame_width  = frame_width;
    fe->frame_pixfmt = frame_pixfmt;
    fe->codec_type   = codec;

    return fe;
}

void frame_extractor_free(frame_extractor_t* frame_extractor)
{
    free(frame_extractor->path);

    for(size_t i=0; i<frame_extractor->n_writers; i++)
        if(frame_extractor->writers[i])
            video_writer_free(frame_extractor->writers[i]);

    free(frame_extractor->track_ids);
    free(frame_extractor->writers);

    free(frame_extractor);
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

                const size_t path_len = frame_extractor->path_len;
                char path_formatted1[path_len];
                char path_formatted2[path_len];

                // Format meteor id placeholder
                char mid[11];
                snprintf(mid, 11, "%ld", frame_extractor->n_meteors++);
                tools_str_format_placeholder(path_formatted1, path_len, frame_extractor->path, "{mid}", mid);

                // Format track id placeholder
                char tid[11];
                snprintf(tid, 11, "%04d", track_id);
                tools_str_format_placeholder(path_formatted2, path_len, path_formatted1, "{tid}", tid);

                // Format frame id placeholder
                tools_str_format_placeholder(path_formatted1, path_len, path_formatted2, "{fid}", "%04d");

                frame_extractor->track_ids[fe_id] = track_id;
                frame_extractor->writers[fe_id] = video_writer_alloc_init(path_formatted1, frame->id,
                                                                          frame_extractor->n_threads,
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

static void _frame_extract_action(frame_t* frame, void* args[]) {
    frame_extract(frame,(frame_extractor_t*)args[0], ((const tracking_data_t*)args[1])->tracks);
}

static void _frame_extract_action_free(void* args[]) {
    frame_extractor_free((frame_extractor_t*)args[0]);
}

void frame_extract_action_register(framebuffer_data_t* framebuffer, const char* path, const size_t n_writers,
                                   const size_t n_threads, const enum video_codec_e codec,
                                   const tracking_data_t* tracking_data) {

    frame_extractor_t* extractor = frame_extractor_alloc_init(path, n_writers,
                                                              framebuffer->frame_height, framebuffer->frame_width,
                                                              n_threads, PIXFMT_RGB24, codec);
    if(!extractor) return;

    framebuffer_action_t* action = framebuffer_action_alloc(2);
    action->apply   = _frame_extract_action;
    action->free    = _frame_extract_action_free;
    action->args[0] = (void*)extractor;
    action->args[1] = (void*)tracking_data;
    framebuffer_action_register(framebuffer, action);
}
