#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <nrc2.h>

#include "vec.h"
#include "fmdt/video/video_io.h"
#include "fmdt/visu/visu_io.h"
#include "fmdt/features/features_compute.h"
#include "fmdt/image/image_compute.h"
#include "fmdt/tracking/tracking_global.h"

visu_data_t* visu_alloc_init(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                             const size_t img_height, const size_t img_width, const enum pixfmt_e pixfmt,
                             const enum video_codec_e codec_type, const uint8_t draw_track_id,
                             const uint8_t draw_legend, const int win_play, const size_t buff_size,
                             const size_t max_RoIs_size) {
    assert(buff_size > 0);
    visu_data_t* visu = (visu_data_t*)malloc(sizeof(visu_data_t));
    visu->img_height = img_height;
    visu->img_width = img_width;
    visu->video_writer = video_writer_alloc_init(path, start, n_ffmpeg_threads, visu->img_height, visu->img_width,
                                                 pixfmt, codec_type, win_play);
    visu->buff_size = buff_size;
    visu->buff_id_read = 0;
    visu->buff_id_write = 0;
    visu->n_filled_buff = 0;
    visu->I = (uint8_t***)malloc(visu->buff_size * sizeof(uint8_t**));
    visu->RoIs = (RoIs_basic_t**)malloc(visu->buff_size * sizeof(RoIs_basic_t**));
    for (size_t i = 0; i < visu->buff_size; i++) {
        visu->I[i] = ui8matrix(0, visu->img_height + 1, 0, visu->img_width + 1);
        visu->RoIs[i] = features_alloc_RoIs_basic(max_RoIs_size, NULL);
    }
    visu->img_data = image_color_alloc(img_height, img_width);
    visu->BBs = (vec_BB_t)vector_create();
    visu->BBs_color = (vec_color_e)vector_create();
    visu->draw_track_id = draw_track_id;
    visu->draw_legend = draw_legend;

    return visu;
}

void _add_to_BB_coord_list(vec_BB_t* BBs, vec_color_e* BBs_color, size_t elem, int rx, int ry, int bb_x, int bb_y,
                           int frame_id, int track_id, int is_extrapolated, enum color_e color) {
    size_t vs = vector_size(*BBs);
    BB_t* BB_elem = (vs == elem) ? vector_add_asg(BBs) : &(*BBs)[elem];
    BB_elem->frame_id = frame_id;
    BB_elem->track_id = track_id;
    BB_elem->bb_x = bb_x;
    BB_elem->bb_y = bb_y;
    BB_elem->rx = rx;
    BB_elem->ry = ry;
    BB_elem->is_extrapolated = is_extrapolated;

    if (vs == elem)
        vector_add(BBs_color, COLOR_MISC);
    enum color_e* BB_color_elem = &(*BBs_color)[elem];
    *BB_color_elem = color;
}

void _visu_write_or_play(visu_data_t* visu, const vec_track_t tracks) {
    const size_t frame_id = visu->buff_id_read;
    const size_t real_buff_id_read = visu->buff_id_read % visu->buff_size;
    int cpt = 0;
    size_t n_tracks = vector_size(tracks);
    for (size_t i = 0; i < n_tracks; i++) {
        const uint32_t track_id = tracks[i].id;
        if (track_id && (tracks[i].end  .frame >= frame_id && tracks[i].begin.frame <= frame_id)) {
            enum color_e color = g_obj_to_color[tracks[i].obj_type];

            const size_t offset = tracks[i].end.frame - frame_id;
            assert(tracks[i].RoIs_id != NULL);
            const size_t RoIs_id_size = vector_size(tracks[i].RoIs_id);
            assert(RoIs_id_size > offset);
            const uint32_t RoI_id = tracks[i].RoIs_id[(RoIs_id_size - 1) - offset];

            RoIs_basic_t *RoIs_tmp = visu->RoIs[real_buff_id_read];
            if (RoI_id) {
                const uint32_t track_x = (uint32_t)roundf(RoIs_tmp->x[RoI_id -1]);
                const uint32_t track_y = (uint32_t)roundf(RoIs_tmp->y[RoI_id -1]);
                const uint32_t track_rx = (RoIs_tmp->xmax[RoI_id -1] - RoIs_tmp->xmin[RoI_id -1]) / 2;
                const uint32_t track_ry = (RoIs_tmp->ymax[RoI_id -1] - RoIs_tmp->ymin[RoI_id -1]) / 2;

                int track_is_extrapolated = 0;
                _add_to_BB_coord_list(&visu->BBs, &visu->BBs_color, cpt, track_rx, track_ry, track_x, track_y, frame_id,
                                      track_id, track_is_extrapolated, color);
                cpt++;
           }
        }
    }

    const int is_gt_path = 0;
    image_color_draw_BBs(visu->img_data, (const uint8_t**)visu->I[real_buff_id_read], (const BB_t*)visu->BBs,
                         (const enum color_e*)visu->BBs_color, cpt, visu->draw_track_id, is_gt_path, visu->draw_legend);

#ifdef FMDT_OPENCV_LINK
    image_color_draw_frame_id(visu->img_data, frame_id);
#endif

    video_writer_save_frame(visu->video_writer, (const uint8_t**)image_color_get_pixels_2d(visu->img_data));
}

void visu_display(visu_data_t* visu, const uint8_t** img, const RoIs_basic_t* RoIs, const vec_track_t tracks) {
    // ------------------------
    // write or play image ----
    // ------------------------
    if (visu->n_filled_buff == visu->buff_size) {
        _visu_write_or_play(visu, tracks);

        visu->n_filled_buff--;
        visu->buff_id_read++;
    }

    // ------------------------
    // bufferize frame --------
    // ------------------------
    assert(visu->n_filled_buff <= visu->buff_size);

    const size_t real_buff_id_write = visu->buff_id_write % visu->buff_size;
    for (size_t i = 0; i < visu->img_height; i++)
        memcpy(visu->I[real_buff_id_write][i], img[i], visu->img_width * sizeof(uint8_t));

    assert(*RoIs->_size <= *visu->RoIs[real_buff_id_write]->_max_size);
    *visu->RoIs[real_buff_id_write]->_size = *RoIs->_size;
    // memcpy(visu->RoIs[real_buff_id_write]->id,   RoIs->id,   *RoIs->_size * sizeof(uint32_t));
    memcpy(visu->RoIs[real_buff_id_write]->xmin, RoIs->xmin, *RoIs->_size * sizeof(uint32_t));
    memcpy(visu->RoIs[real_buff_id_write]->xmax, RoIs->xmax, *RoIs->_size * sizeof(uint32_t));
    memcpy(visu->RoIs[real_buff_id_write]->ymin, RoIs->ymin, *RoIs->_size * sizeof(uint32_t));
    memcpy(visu->RoIs[real_buff_id_write]->ymax, RoIs->ymax, *RoIs->_size * sizeof(uint32_t));
    memcpy(visu->RoIs[real_buff_id_write]->x,    RoIs->x,    *RoIs->_size * sizeof(float));
    memcpy(visu->RoIs[real_buff_id_write]->y,    RoIs->y,    *RoIs->_size * sizeof(float));

    visu->n_filled_buff++;
    visu->buff_id_write++;
}

void visu_flush(visu_data_t* visu, const vec_track_t tracks) {
    while (visu->n_filled_buff) {
        _visu_write_or_play(visu, tracks);

        visu->n_filled_buff--;
        visu->buff_id_read++;
    };
}

void visu_free(visu_data_t* visu) {
    video_writer_free(visu->video_writer);
    for (size_t i = 0; i < visu->buff_size; i++) {
        free_ui8matrix(visu->I[i], 0, visu->img_height + 1, 0, visu->img_width + 1);
        features_free_RoIs_basic(visu->RoIs[i], 1);
    }
    free(visu->I);
    free(visu->RoIs);
    image_color_free(visu->img_data);
    vector_free(visu->BBs);
    vector_free(visu->BBs_color);
    free(visu);
}
