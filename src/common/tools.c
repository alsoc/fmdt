#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef OPENCV_LINK
#include <tuple>
#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#endif
#include <ffmpeg-io/reader.h>
#include <ffmpeg-io/writer.h>
#include <nrc2.h>

#include "fmdt/macros.h"
#include "fmdt/tools.h"
#include "vec.h"

#define DELTA_BB 5 // extra pixel size for bounding boxes

rgb8_t tools_get_color(enum color_e color) {
    rgb8_t gray;
    gray.g = 175;
    gray.b = 175;
    gray.r = 175;

    rgb8_t green;
    green.g = 255;
    green.b = 000;
    green.r = 000;

    rgb8_t red;
    red.g = 000;
    red.b = 000;
    red.r = 255;

    rgb8_t blue;
    blue.g = 000;
    blue.b = 255;
    blue.r = 000;

    rgb8_t purple;
    purple.g = 127;
    purple.b = 255;
    purple.r = 127;

    rgb8_t orange;
    orange.r = 255;
    orange.g = 165;
    orange.b = 000;

    rgb8_t yellow;
    yellow.g = 255;
    yellow.b = 000;
    yellow.r = 255;

    rgb8_t misc;
    misc.g = 255;
    misc.b = 153;
    misc.r = 153;

    switch (color) {
    case GRAY:
        return gray;
    case GREEN:
        return green;
    case RED:
        return red;
    case BLUE:
        return blue;
    case PURPLE:
        return purple;
    case ORANGE:
        return orange;
    case YELLOW:
        return yellow;
    case MISC:
        return misc;
    default:
        break;
    }
    return red;
}

void tools_plot_bounding_box(rgb8_t* img, const size_t width, int ymin, int ymax, int xmin, int xmax, int border,
                             rgb8_t color, int is_dashed) {
#ifdef OPENCV_LINK
    rgb8_t color_tmp = color;
    color.r = color_tmp.b;
    color.b = color_tmp.r;
#endif
    for (int b = 0; b < border; b++) {
        ymin++;
        ymax--;
        xmin++;
        xmax--;

        const int limit = 3;
        int counter = b % limit;
        int draw = 1;

        for (int i = ymin; i <= ymax; i++) {
            if (draw) {
                img[i * width + xmin] = color;
                img[i * width + xmax] = color;
            }

            if (is_dashed) {
                counter++;
                int draw_before = draw;
                draw = counter == limit ? !draw : draw;
                if (draw != draw_before)
                    counter = 0;
            }
        }

        counter = b % limit;
        draw = 1;
        for (int j = xmin; j <= xmax; j++) {
            if (draw) {
                img[ymin * width + j] = color;
                img[ymax * width + j] = color;
            }

            if (is_dashed) {
                counter++;
                int draw_before = draw;
                draw = counter == limit ? !draw : draw;
                if (draw != draw_before)
                    counter = 0;
            }
        }
    }
}

#ifdef OPENCV_LINK // this is C++ code (because OpenCV API is C++ now)
void tools_draw_legend_squares(rgb8_t* img, const size_t width, unsigned box_size, unsigned h_space, unsigned v_space,
                               int validation) {
    //                     ymin      ymax      xmin      xmax      color
    std::vector<std::tuple<unsigned, unsigned, unsigned, unsigned, rgb8_t>> box_list;

    for (int i = 0; i < N_OBJECTS; i++)
        box_list.push_back(std::make_tuple((i + 1) * v_space + (i + 0) * box_size, // ymin
                                           (i + 1) * v_space + (i + 1) * box_size, // ymax
                                           (+1) * h_space + (+0) * box_size,       // xmin
                                           (+1) * h_space + (+1) * box_size,       // xmax
                                           tools_get_color(g_obj_to_color[i])));   // color

    if (validation)
        // add false positive meteor
        box_list.push_back(std::make_tuple((N_OBJECTS + 2) * v_space + (N_OBJECTS + 1) * box_size, // ymin
                                           (N_OBJECTS + 2) * v_space + (N_OBJECTS + 2) * box_size, // ymax
                                           (+1) * h_space + (+0) * box_size,                       // xmin
                                           (+1) * h_space + (+1) * box_size,                       // xmax
                                           tools_get_color(RED)));                                 // color

    for (auto& box : box_list)
        tools_plot_bounding_box(img, width, std::get<0>(box), std::get<1>(box), std::get<2>(box), std::get<3>(box), 2,
                                std::get<4>(box), /* is_dashed = */ 0);
}

void tools_draw_legend_text(cv::Mat& cv_img, unsigned box_size, unsigned h_space, unsigned v_space, int validation) {
    //                          color        pos         text
    std::vector<std::tuple<cv::Scalar, cv::Point, std::string>> txt_list;
    for (int i = 0; i < N_OBJECTS; i++) {
        rgb8_t color = tools_get_color(g_obj_to_color[i]);
        unsigned x = 2 * h_space + box_size;
        unsigned y = ((i + 1) * v_space + (i + 1) * box_size) - 2;
        txt_list.push_back(std::make_tuple(cv::Scalar(color.b, color.g, color.r), cv::Point(x, y),
                                           std::string(g_obj_to_string[i])));
    }

    if (validation) {
        // add false positve meteor
        rgb8_t color = tools_get_color(RED);
        unsigned x = 2 * h_space + box_size;
        unsigned y = ((N_OBJECTS + 2) * v_space + (N_OBJECTS + 2) * box_size) - 2;
        txt_list.push_back(
            std::make_tuple(cv::Scalar(color.b, color.g, color.r), cv::Point(x, y), std::string("fp meteor")));
    }

    for (auto& txt : txt_list)
        cv::putText(cv_img,
                    std::get<2>(txt).c_str(), // text
                    std::get<1>(txt),         // position
                    cv::FONT_HERSHEY_DUPLEX,  // font type
                    0.7,                      // font size
                    std::get<0>(txt),         // color
                    1,                        // ?
                    cv::LINE_AA);             // ?
}

void tools_draw_track_id(cv::Mat& cv_img, const BB_t* BB_list, const enum color_e* BB_list_color, const int nBB) {
    //                       x    y color        list of ids
    std::vector<std::tuple<int, int, rgb8_t, std::vector<int>>> list_of_ids_grouped_by_pos;
    for (int i = 0; i < nBB; i++) {
        int ymin = BB_list[i].bb_y - (BB_list[i].ry + DELTA_BB);
        int ymax = BB_list[i].bb_y + (BB_list[i].ry + DELTA_BB);
        // int xmin = BB_list[i].bb_x - (BB_list[i].rx + DELTA_BB);
        int xmax = BB_list[i].bb_x + (BB_list[i].rx + DELTA_BB);

        int x = xmax + 3;
        int y = (ymin) + ((ymax - ymin) / 2);

        bool found = false;
        for (auto& l : list_of_ids_grouped_by_pos) {
            rgb8_t c = tools_get_color(BB_list_color[i]);
            if (std::get<0>(l) == x && std::get<1>(l) == y && std::get<2>(l).r == c.r && std::get<2>(l).g == c.g &&
                std::get<2>(l).b == c.b) {
                std::get<3>(l).push_back(BB_list[i].track_id);
                found = true;
            }
        }

        if (!found) {
            std::vector<int> v;
            v.push_back(BB_list[i].track_id);
            list_of_ids_grouped_by_pos.push_back(std::make_tuple(x, y, tools_get_color(BB_list_color[i]), v));
        }
    }

    for (auto id : list_of_ids_grouped_by_pos) {
        std::string txt = std::to_string(std::get<3>(id)[std::get<3>(id).size() - 1]);
        for (int s = std::get<3>(id).size() - 2; s >= 0; s--)
            txt += "," + std::to_string(std::get<3>(id)[s]);

        const int x = std::get<0>(id);
        const int y = std::get<1>(id);
        const rgb8_t color = std::get<2>(id);

        // writing 'txt' over the image
        cv::Point org(x, y);
        cv::putText(cv_img, txt.c_str(), org, cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(color.b, color.g, color.r), 1,
                    cv::LINE_AA);
    }
}

void tools_draw_text(cv::Mat* pixels, const BB_t* BB_list, const enum color_e* BB_list_color, const int nBB,
                     int validation, int show_id) {
    unsigned box_size = 20, h_space = 10, v_space = 10;
    tools_draw_legend_squares((rgb8_t*)pixels->data, pixels->cols, box_size, h_space, v_space, validation);

    if (show_id)
        tools_draw_track_id(*pixels, BB_list, BB_list_color, nBB);
    tools_draw_legend_text(*pixels, box_size, h_space, v_space, validation);
}

void _tools_draw_ROI_ids(cv::Mat& cv_img, const uint32_t* ROI_id, const uint32_t* ROI_xmax, const uint32_t* ROI_ymin,
                         const uint32_t* ROI_ymax, const size_t n_ROI) {
    //                       x    y  list of ids
    std::vector<std::tuple<int, int, std::vector<int>>> list_of_ids_grouped_by_pos;
    for (size_t i = 0; i < n_ROI; i++) {
        int x = ROI_xmax[i] + 3;
        int y = ROI_ymin[i] + (ROI_ymax[i] - ROI_ymin[i]) / 2;

        bool found = false;
        for (auto& l : list_of_ids_grouped_by_pos) {
            if (std::get<0>(l) == x && std::get<1>(l) == y) {
                std::get<2>(l).push_back(ROI_id[i]);
                found = true;
            }
        }

        if (!found) {
            std::vector<int> v;
            v.push_back(ROI_id[i]);
            list_of_ids_grouped_by_pos.push_back(std::make_tuple(x, y, v));
        }
    }

    for (auto id : list_of_ids_grouped_by_pos) {
        std::string txt = std::to_string(std::get<2>(id)[std::get<2>(id).size() - 1]);
        for (int s = std::get<2>(id).size() - 2; s >= 0; s--)
            txt += "," + std::to_string(std::get<2>(id)[s]);

        const int x = std::get<0>(id);
        const int y = std::get<1>(id);

        // writing 'txt' over the image
        cv::Point org(x, y);
        cv::putText(cv_img, txt.c_str(), org, cv::FONT_HERSHEY_DUPLEX, 0.7, 255, 1, cv::LINE_AA);
    }
}
#endif

void tools_draw_BB(rgb8_t* I_bb, const BB_t* BB_list, const enum color_e* BB_list_color, int n_BB, int w, int h) {
    int border = 2;
    for (int i = 0; i < n_BB; i++) {
        int ymin = BB_list[i].bb_y - (BB_list[i].ry + DELTA_BB);
        int ymax = BB_list[i].bb_y + (BB_list[i].ry + DELTA_BB);
        int xmin = BB_list[i].bb_x - (BB_list[i].rx + DELTA_BB);
        int xmax = BB_list[i].bb_x + (BB_list[i].rx + DELTA_BB);

        int ymin_fix = CLAMP(ymin, border + 1, h - (border + 2));
        int ymax_fix = CLAMP(ymax, border + 1, h - (border + 2));
        int xmin_fix = CLAMP(xmin, border + 1, w - (border + 2));
        int xmax_fix = CLAMP(xmax, border + 1, w - (border + 2));

        tools_plot_bounding_box(I_bb, w, ymin_fix, ymax_fix, xmin_fix, xmax_fix, border, tools_get_color(BB_list_color[i]),
                                BB_list[i].is_extrapolated);
    }
}

void tools_save_bounding_box(const char* filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame) {
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }
    fprintf(f, "%4d \t %4d \t %4d \t %4d \t %4d \n", frame, rx, ry, bb_x, bb_y);
    fclose(f);
}

void tools_filter_speed_binarize(uint32** in, int i0, int i1, int j0, int j1, uint8** out, ROI_t* stats) {
    for (int i = i0; i <= i1; i++)
        for (int j = j0; j <= j1; j++)
            out[i][j] = stats[in[i][j]].S ? 0xFF : 0;
}

rgb8_t** tools_load_image_color(const char* filename, long* i0, long* i1, long* j0, long* j1) {
    VERBOSE(printf("%s\n", filename););
    ffmpeg_handle reader;
    ffmpeg_init(&reader);
    ffmpeg_options options;
    ffmpeg_options_init(&options);

    options.debug = 1;

    if (!ffmpeg_probe(&reader, filename, &options)) {
        fprintf(stderr, "(EE) Error in load_image_color: %s (%d)\n", ffmpeg_error2str(reader.error), reader.error);
        exit(0);
    }
    reader.output.pixfmt = ffmpeg_str2pixfmt("rgb24");
    *i0 = 0;
    *j0 = 0;
    *i1 = reader.input.height - 1;
    *j1 = reader.input.width - 1;
    VERBOSE(printf("i1=%ld j1=%ld\n", *i1, *j1););
    rgb8_t** img = (rgb8_t**)rgb8matrix(*i0, *i1, *j0, *j1);

    ffmpeg_start_reader(&reader, filename, NULL);
    ffmpeg_read2d(&reader, (uint8_t**)img);
    ffmpeg_stop_reader(&reader);
    return img;
}

void tools_save_frame_threshold(const char* filename, uint8** I0, uint8** I1, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8_t** img = (rgb8_t**)rgb8matrix(0, h - 1, 0, 2 * w - 1);
    if (img == NULL)
        return;

    // (0,0) : video
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage luminosité
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j + w].r = I1[i][j];
            img[i][j + w].g = I1[i][j];
            img[i][j + w].b = I1[i][j];
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */

    snprintf(buffer, sizeof(buffer), "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        tools_write_PNM_row((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix((rgb8**)img, 0, h - 1, 0, 2 * w - 1);
}

void tools_HSV_to_RGB(rgb8_t* pixel, uint8 h, uint8 s, uint8 v) {
    unsigned char region, remainder, p, q, t;

    if (s == 0) {
        pixel->r = v;
        pixel->g = v;
        pixel->b = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
    case 0:
        pixel->r = v;
        pixel->g = t;
        pixel->b = p;
        break;
    case 1:
        pixel->r = q;
        pixel->g = v;
        pixel->b = p;
        break;
    case 2:
        pixel->r = p;
        pixel->g = v;
        pixel->b = t;
        break;
    case 3:
        pixel->r = p;
        pixel->g = q;
        pixel->b = v;
        break;
    case 4:
        pixel->r = t;
        pixel->g = p;
        pixel->b = v;
        break;
    default:
        pixel->r = v;
        pixel->g = p;
        pixel->b = q;
        break;
    }
}

void tools_save_frame_quad(const char* filename, uint8** I0, uint8** I1, uint32** I2, uint32** I3, int nbLabel,
                           ROI_t* stats, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8_t** img = (rgb8_t**)rgb8matrix(0, 2 * h - 1, 0, 2 * w - 1);
    if (img == NULL)
        return;

    // (0,0) : video
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage luminosité
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j + w].r = I1[i][j];
            img[i][j + w].g = I1[i][j];
            img[i][j + w].b = I1[i][j];
        }
    }
    // (1,0) : CCL
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (I2[i][j] != 0) {
                tools_HSV_to_RGB((img[i + h]) + j, (uint8)(I2[i][j] * 360 / 5), 255, 255);
            } else {
                tools_HSV_to_RGB((img[i + h]) + j, 0, 0, 0);
            }
        }
    }
    // Carrés vert sur les CC
    // for (int k=1 ; k<=nbLabel ; k++) {
    //     if (stats[k].S > 3) {
    //         rgb8_t color;
    //         //tools_HSV_to_RGB(&color, k*255 / nbLabel, 255, 255);
    //         color.r = 0;
    //         color.g = 255;
    //         color.b = 0;
    //         int ymin = MIN(stats[k].ymin + h - 10, h*2-1);
    //         int ymax = MIN(stats[k].ymax + h + 10, h*2-1);
    //         int xmin = MIN(stats[k].xmin     - 10, w  -1);
    //         int xmax = MIN(stats[k].xmax     + 10, w  -1);

    //         for (int b=0; b<5 ; b++) {
    //             ymin++; ymax--;xmin++;xmax--;
    //             for (int i=ymin ; i<=ymax ; i++) {
    //                 img[i][xmin] = color;
    //                 img[i][xmax] = color;
    //             }
    //             for (int j=xmin ; j<=xmax ; j++) {
    //                 img[ymin][j] = color;
    //                 img[ymax][j] = color;
    //             }
    //         }
    //     }
    // }

    // (1,1) : filtrage surface
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i + h][j + w].r = (I3[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].g = (I3[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].b = (I3[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */

    snprintf(buffer, sizeof(buffer), "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= 2 * h - 1; i++)
        tools_write_PNM_row((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix((rgb8**)img, 0, 2 * h - 1, 0, 2 * w - 1);
}

void tools_save_frame_quad_hysteresis(const char* filename, uint8** I0, uint32** SH, uint32** SB, uint32** Y, int i0,
                                      int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8_t** img = (rgb8_t**)rgb8matrix(0, 2 * h - 1, 0, 2 * w - 1);
    if (img == NULL)
        return;

    // (0,0) : video
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage low
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (SB[i][j] != 0) {
                tools_HSV_to_RGB((img[i]) + j + w, (uint8)(SB[i][j] * 360 / 5), 255, 255);
            } else {
                tools_HSV_to_RGB((img[i]) + j + w, 0, 0, 0);
            }
        }
    }

    // (0,1) : seuillage high
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (SH[i][j] != 0) {
                tools_HSV_to_RGB((img[i + h]) + j, (uint8)(SH[i][j] * 360 / 5), 255, 255);
            } else {
                tools_HSV_to_RGB((img[i + h]) + j, 0, 0, 0);
            }
        }
    }

    // (0,0) : out
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i + h][j + w].r = (Y[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].g = (Y[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].b = (Y[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */

    snprintf(buffer, sizeof(buffer), "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= 2 * h - 1; i++)
        tools_write_PNM_row((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix((rgb8**)img, 0, 2 * h - 1, 0, 2 * w - 1);
}

void tools_save_max(const char* filename, uint8** I, int i0, int i1, int j0, int j1) {
    uint8 m;
    uint8* res = ui8vector(i0, i1);

    zero_ui8vector(res, i0, i1);

    for (int i = i0; i <= i1; i++) {
        m = 0;
        for (int j = j0; j <= j1; j++) {
            m = MAX(m, I[i][j]);
        }
        res[i] = m;
    }

    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = i1; i >= i0; i--) {
        fprintf(f, "%4d\t%4d\n", i, res[i]);
    }
    fclose(f);
}

void tools_create_folder(const char* folder_path) {
    struct stat status = {0};
    if (stat(folder_path, &status) == -1)
        mkdir(folder_path, 0700);
}

void tools_copy_ui8matrix_ui8matrix(const uint8** X, const int i0, const int i1, const int j0, const int j1,
                                    uint8** Y) {
    for (int i = i0; i <= i1; i++)
        memcpy(Y[i] + j0, X[i] + j0, sizeof(uint8_t) * ((j1 - j0) + 1));
}

void tools_convert_ui8vector_ui32vector(const uint8* X, const long nl, const long nh, uint32* Y) {
    for (long i = nl; i <= nh; i++)
        Y[i] = (uint32)X[i];
}

// void tools_convert_ui8matrix_ui32matrix(const uint8** X, const int nrl, const int nrh, const int ncl, const int nch,
//                                         uint32** Y) {
//     for (long i = nrl; i <= nrh; i++)
//         tools_convert_ui8vector_ui32vector(X[i], ncl, nch, Y[i]);
// }

void tools_convert_ui8matrix_ui32matrix(const uint8** X, const int nrl, const int nrh, const int ncl, const int nch,
                                        uint32** Y) {
    for (long i = nrl; i <= nrh; i++)
        for (long j = ncl; j <= nch; j++)
            Y[i][j] = (uint32)X[i][j];
}

void tools_write_PNM_row(const uint8* line, const int width, FILE* file) {
    /* Le fichier est deja ouvert et ne sera pas ferme a la fin */
    fwrite(&(line[0]), sizeof(byte), 3 * sizeof(byte) * width, file);
}

#define TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y) \
    assert(X != NULL);                            \
    assert(Y != NULL);                            \
    assert(i0 <= 0);                              \
    assert(j0 <= 0);                              \
    assert(i1 > i0 && i1 > 0);                    \
    assert(j1 > j0 && j1 > 0);                    \
    Y[i0] = X - j0;                               \
    for (int i = i0 + 1; i <= i1 ; i++)           \
        Y[i] = Y[i - 1] + ((j1 - j0) + 1);

void tools_linear_2d_nrc_ui8matrix(const uint8_t* X, const int i0, const int i1, const int j0, const int j1, 
                         const uint8_t** Y) {
    TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y)
}

void tools_linear_2d_nrc_ui32matrix(const uint32_t* X, const int i0, const int i1, const int j0, const int j1, 
                         const uint32_t** Y) {
    TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y)
}

void tools_linear_2d_nrc_f32matrix(const float* X, const int i0, const int i1, const int j0, const int j1, 
                                   const float** Y) {
    TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y)
}

void tools_linear_2d_nrc_rgb8matrix(const rgb8_t* X, const int i0, const int i1, const int j0, const int j1,
                                    const rgb8_t** Y) {
    TOOLS_LINEAR_2D_NRC(X, i0, i1, j0, j1, Y)
}

int tools_is_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

img_data_t* tools_gray_img_alloc1(const size_t img_width, const size_t img_height, const char* path, const char* ext,
                                  const uint8_t show_id) {
    img_data_t* img_data = (img_data_t*)malloc(sizeof(img_data_t));
    snprintf(img_data->ext, sizeof(img_data->ext), "%s", ext);
    snprintf(img_data->path, sizeof(img_data->path), "%s", path);
    if (strlen(path))
        tools_create_folder(img_data->path);
    img_data->show_id = show_id;
#ifndef OPENCV_LINK
    if (strcmp(img_data->ext, "pgm") != 0) {
        fprintf(stderr, "(EE) 'gray_img' only supports 'pgm' image format, you need to link with OpenCV "
                        "to unlock other image formats.");
        exit(1);
    }
    if (show_id) {
        fprintf(stderr, "(EE) 'gray_img' does not support 'show_id = 1', you need to link with OpenCV "
                        "to unlock this feature.");
        exit(1);
    }
#endif
    img_data->width = img_width;
    img_data->height = img_height;
#ifdef OPENCV_LINK
    img_data->pixels = (void*) new cv::Mat(img_data->height, img_data->width, CV_8U, cv::Scalar(255));
    uint8_t** container_2d = (uint8_t**) malloc(sizeof(uint8_t*) * img_data->height);
    tools_linear_2d_nrc_ui8matrix((const uint8_t*)tools_gray_img_get_pixels(img_data), 0, img_data->height - 1,
                                  0, img_data->width - 1, (const uint8_t**)container_2d);
    img_data->container_2d = (void*)container_2d;
#else
    img_data->pixels = (void*) ui8matrix(0, img_data->height -1, 0, img_data->width -1);
    img_data->container_2d = img_data->pixels;
#endif
    return img_data;
}

img_data_t* tools_gray_img_alloc2(const size_t img_width, const size_t img_height, const char* ext,
                                  const uint8_t show_id) {
    return tools_gray_img_alloc1(img_width, img_height, "", ext, show_id);
}

void _tools_gray_img_draw_labels(img_data_t* img_data, const uint32_t** labels, const uint32_t* ROI_id,
                                 const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                                 const size_t n_ROI) {
#ifdef OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    // convert labels to black & white image: white if there is a CC, black otherwise
    for (size_t i = 0; i < (size_t)pixels->rows; i++)
        for (size_t j = 0; j < (size_t)pixels->cols; j++)
            pixels->at<uint8_t>(i, j) = (labels[i][j] == 0) ? 0 : 255;
    if (img_data->show_id)
        _tools_draw_ROI_ids(*pixels, ROI_id, ROI_xmax, ROI_ymin, ROI_ymax, n_ROI);
#else
    uint8_t** pixels = (uint8_t**)img_data->pixels;
    // convert labels to black & white image: white if there is a CC, black otherwise
    for (size_t i = 0; i < img_data->height; i++)
        for (size_t j = 0; j < img_data->width; j++)
            pixels[i][j] = (labels[i][j] == 0) ? 0 : 255;
#endif
}

void tools_gray_img_draw_labels(img_data_t* img_data, const uint32_t** labels, const ROI_t* ROI_array) {
    _tools_gray_img_draw_labels(img_data, labels, ROI_array->id, ROI_array->xmax, ROI_array->ymin, ROI_array->ymax,
                                ROI_array->_size);
}

uint8_t* tools_gray_img_get_pixels(img_data_t* img_data) {
    uint8_t* raw_data = NULL;
#ifdef OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    raw_data = (uint8_t*)pixels->data;
#else
    uint8_t** pixels = (uint8_t**)img_data->pixels;
    raw_data = pixels[0];
#endif
    return raw_data;
}

uint8_t** tools_gray_img_get_pixels_2d(img_data_t* img_data) {
    return (uint8_t**)img_data->container_2d;
}

void _tools_gray_img_write(img_data_t* img_data, const char* filename) {
#ifdef OPENCV_LINK
    if (strcmp(img_data->ext, "pgm") == 0) {
        uint8_t** pixels = tools_gray_img_get_pixels_2d(img_data);
        SavePGM_ui8matrix((uint8**)pixels, 0, img_data->height -1, 0, img_data->width -1, (char*)filename);
    } else {
        cv::Mat* pixels = (cv::Mat*)img_data->pixels;
        cv::imwrite(filename, *pixels);
    }
#else
    uint8_t** pixels = (uint8_t**)img_data->pixels;
    SavePGM_ui8matrix((uint8**)pixels, 0, img_data->height -1, 0, img_data->width -1, (char*)filename);
#endif
}

void tools_gray_img_write1(img_data_t* img_data, const size_t frame) {
    char filename[2048];
    snprintf(filename, sizeof(filename), "%s/%05d.%s", img_data->path, (int)frame, img_data->ext);
    _tools_gray_img_write(img_data, filename);
}

void tools_gray_img_write2(img_data_t* img_data, const char* filename) {
    char filename_with_ext[2048];
    snprintf(filename_with_ext, sizeof(filename_with_ext), "%s.%s", filename, img_data->ext);
    _tools_gray_img_write(img_data, filename_with_ext);
}

void tools_gray_img_free(img_data_t* img_data) {
#ifdef OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    delete pixels;
    free((uint8_t**)img_data->container_2d);
#else
    uint8_t** pixels = (uint8_t**)img_data->pixels;
    free_ui8matrix(pixels, 0, img_data->height -1, 0, img_data->width -1);
#endif
    free(img_data);
}

img_data_t* tools_color_img_alloc1(const size_t img_width, const size_t img_height, const char* path, const char* ext,
                                   const uint8_t show_id) {
    img_data_t* img_data = (img_data_t*)malloc(sizeof(img_data_t));
    snprintf(img_data->ext, sizeof(img_data->ext), "%s", ext);
    snprintf(img_data->path, sizeof(img_data->path), "%s", path);

    if (strlen(path))
        tools_create_folder(img_data->path);
    img_data->show_id = show_id;
#ifndef OPENCV_LINK
    if (strcmp(img_data->ext, "ppm") != 0) {
        fprintf(stderr, "(EE) 'color_img' only supports 'ppm' image format, you need to link with OpenCV "
                        "to unlock other image formats.");
        exit(1);
    }
    if (show_id) {
        fprintf(stderr, "(EE) 'color_img' does not support 'show_id = 1', you need to link with OpenCV "
                        "to unlock this feature.");
        exit(1);
    }
#endif
    img_data->width = img_width;
    img_data->height = img_height;
#ifdef OPENCV_LINK
    img_data->pixels = (void*) new cv::Mat(img_data->height, img_data->width, CV_8UC3, cv::Scalar(255, 255, 255));
    rgb8_t** container_2d = (rgb8_t**) malloc(sizeof(rgb8_t*) * img_data->height);
    tools_linear_2d_nrc_rgb8matrix((const rgb8_t*)tools_color_img_get_pixels(img_data), 0, img_data->height - 1, 0,
                                   img_data->width - 1, (const rgb8_t**)container_2d);
    img_data->container_2d = (void*)container_2d;
#else
    img_data->pixels = (void*) rgb8matrix(0, img_data->height -1, 0, img_data->width -1);
    img_data->container_2d = img_data->pixels;
#endif
    return img_data;
}

img_data_t* tools_color_img_alloc2(const size_t img_width, const size_t img_height, const char* ext,
                                   const uint8_t show_id) {
    return tools_color_img_alloc1(img_width, img_height, "", ext, show_id);
}

void tools_color_img_draw_BB(img_data_t* img_data, const uint8_t** img, const BB_t* BB_list,
                             const enum color_e* BB_list_color, const size_t n_BB, const uint8_t is_gt) {
    rgb8_t* raw_data = NULL;
#ifdef OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    for (size_t i = 0; i < (size_t)pixels->rows; i++) {
        for (size_t j = 0; j < (size_t)pixels->cols; j++) {
            pixels->at<cv::Vec3b>(i, j)[2] = img[i][j];
            pixels->at<cv::Vec3b>(i, j)[1] = img[i][j];
            pixels->at<cv::Vec3b>(i, j)[0] = img[i][j];
        }
    }
    raw_data = (rgb8_t*)pixels->data;
#else
    rgb8_t** pixels = (rgb8_t**)img_data->pixels;
    for (size_t i = 0; i < img_data->height; i++) {
        for (size_t j = 0; j < img_data->width; j++) {
            pixels[i][j].r = img[i][j];
            pixels[i][j].g = img[i][j];
            pixels[i][j].b = img[i][j];
        }
    }
    raw_data = pixels[0];
#endif
    tools_draw_BB(raw_data, BB_list, BB_list_color, n_BB, img_data->width, img_data->height);
#ifdef OPENCV_LINK
    tools_draw_text(pixels, BB_list, BB_list_color, n_BB, is_gt, img_data->show_id);
#endif
}

rgb8_t* tools_color_img_get_pixels(img_data_t* img_data) {
    rgb8_t* raw_data = NULL;
#ifdef OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    raw_data = (rgb8_t*)pixels->data;
#else
    rgb8_t** pixels = (rgb8_t**)img_data->pixels;
    raw_data = pixels[0];
#endif
    return raw_data;
}

rgb8_t** tools_color_img_get_pixels_2d(img_data_t* img_data) {
    return (rgb8_t**)img_data->container_2d;
}

void _tools_color_img_write(img_data_t* img_data, const char* filepath) {
#ifdef OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    cv::imwrite(filepath, *pixels);
#else
    rgb8_t** pixels = (rgb8_t**)img_data->pixels;
    SavePPM_rgb8matrix((rgb8**)pixels, 0, img_data->height -1, 0, img_data->width -1, (char*)filepath);
#endif
}

void tools_color_img_write1(img_data_t* img_data, const size_t frame) {
    char filename[2048];
    snprintf(filename, sizeof(filename), "%s/%05d.%s", img_data->path, (int)frame, img_data->ext);
    _tools_color_img_write(img_data, filename);
}

void tools_color_img_write2(img_data_t* img_data, const char* filename) {
    char filename_with_ext[2048];
    snprintf(filename_with_ext, sizeof(filename_with_ext), "%s.%s", filename, img_data->ext);
    _tools_color_img_write(img_data, filename_with_ext);
}

void tools_color_img_free(img_data_t* img_data) {
#ifdef OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    delete pixels;
    free((rgb8_t**)img_data->container_2d);
#else
    rgb8_t** pixels = (rgb8_t**)img_data->pixels;
    free_rgb8matrix((rgb8**)pixels, 0, img_data->height -1, 0, img_data->width -1);
#endif
    free(img_data);
}
