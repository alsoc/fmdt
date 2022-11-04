#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

#ifdef OPENCV_LINK // this is C++ code (because OpenCV API is C++ now)
void tools_draw_legend_squares(rgb8_t** img, unsigned box_size, unsigned h_space, unsigned v_space, int validation) {
    //                     ymin      ymax      xmin      xmax     color
    std::vector<std::tuple<unsigned, unsigned, unsigned, unsigned, rgb8_t>> box_list;

    for (int i = 0; i < N_OBJECTS; i++)
        box_list.push_back(std::make_tuple((i + 1) * v_space + (i + 0) * box_size,    // ymin
                                           (i + 1) * v_space + (i + 1) * box_size,    // ymax
                                           (+1) * h_space + (+0) * box_size,          // xmin
                                           (+1) * h_space + (+1) * box_size,          // xmax
                                           tools_get_color(g_obj_to_color[i]))); // color

    if (validation)
        // add false positve meteor
        box_list.push_back(std::make_tuple((N_OBJECTS + 2) * v_space + (N_OBJECTS + 1) * box_size, // ymin
                                           (N_OBJECTS + 2) * v_space + (N_OBJECTS + 2) * box_size, // ymax
                                           (+1) * h_space + (+0) * box_size,                       // xmin
                                           (+1) * h_space + (+1) * box_size,                       // xmax
                                           tools_get_color(RED)));                                 // color

    for (auto& box : box_list)
        tools_plot_bounding_box(img, std::get<0>(box), std::get<1>(box), std::get<2>(box), std::get<3>(box), 2,
                                std::get<4>(box));
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

void tools_draw_track_id(cv::Mat& cv_img, const BB_coord_t* listBB, const int nBB) {
    //                       x    y color        list of ids
    std::vector<std::tuple<int, int, rgb8_t, std::vector<int>>> list_of_ids_grouped_by_pos;
    for (int i = 0; i < nBB; i++) {
        int x = listBB[i].xmax + 3;
        int y = (listBB[i].ymin) + ((listBB[i].ymax - listBB[i].ymin) / 2);

        bool found = false;
        for (auto& l : list_of_ids_grouped_by_pos) {
            rgb8_t c = tools_get_color(listBB[i].color);
            if (std::get<0>(l) == x && std::get<1>(l) == y && std::get<2>(l).r == c.r && std::get<2>(l).g == c.g &&
                std::get<2>(l).b == c.b) {
                std::get<3>(l).push_back(listBB[i].track_id);
                found = true;
            }
        }

        if (!found) {
            std::vector<int> v;
            v.push_back(listBB[i].track_id);
            list_of_ids_grouped_by_pos.push_back(std::make_tuple(x, y, tools_get_color(listBB[i].color), v));
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

void tools_draw_text(rgb8_t** img, const int img_width, const int img_height, const BB_coord_t* listBB, const int nBB,
                     int validation, int show_id) {
    unsigned box_size = 20, h_space = 10, v_space = 10;
    tools_draw_legend_squares(img, box_size, h_space, v_space, validation);

    // create a blank image of size
    // (img_width x img_height) with white background
    // (B, G, R) : (255, 255, 255)
    cv::Mat cv_img(img_height, img_width, CV_8UC3, cv::Scalar(255, 255, 255));

    // check if the image is created successfully
    if (!cv_img.data) {
        std::cerr << "(EE) Could not open or find the image" << std::endl;
        std::exit(-1);
    }

    // convert: 'img' into 'cv::Mat'
    for (int i = 0; i < img_height; i++)
        for (int j = 0; j < img_width; j++) {
            cv_img.at<cv::Vec3b>(i, j)[2] = img[i][j].r;
            cv_img.at<cv::Vec3b>(i, j)[1] = img[i][j].g;
            cv_img.at<cv::Vec3b>(i, j)[0] = img[i][j].b;
        }

    if (show_id)
        tools_draw_track_id(cv_img, listBB, nBB);
    tools_draw_legend_text(cv_img, box_size, h_space, v_space, validation);

    // // debug: show image inside a window.
    // cv::imshow("Output", cv_img);
    // cv::waitKey(0);

    // convert back: 'cv::Mat' into 'img'
    for (int i = 0; i < img_height; i++) {
        for (int j = 0; j < img_width; j++) {
            img[i][j].r = cv_img.at<cv::Vec3b>(i, j)[2];
            img[i][j].g = cv_img.at<cv::Vec3b>(i, j)[1];
            img[i][j].b = cv_img.at<cv::Vec3b>(i, j)[0];
        }
    }
}
#endif

void tools_convert_img_grayscale_to_rgb(const uint8** I, rgb8_t** I_bb, int i0, int i1, int j0, int j1) {
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            I_bb[i][j].r = I[i][j];
            I_bb[i][j].g = I[i][j];
            I_bb[i][j].b = I[i][j];
        }
    }
}

void tools_draw_BB(rgb8_t** I_bb, const BB_coord_t* listBB, int n_BB, int w, int h) {
    int border = 2;
    for (int i = 0; i < n_BB; i++) {
        int ymin = CLAMP(listBB[i].ymin, border + 1, h - (border + 2));
        int ymax = CLAMP(listBB[i].ymax, border + 1, h - (border + 2));
        int xmin = CLAMP(listBB[i].xmin, border + 1, w - (border + 2));
        int xmax = CLAMP(listBB[i].xmax, border + 1, w - (border + 2));
        tools_plot_bounding_box(I_bb, ymin, ymax, xmin, xmax, border, tools_get_color(listBB[i].color));
    }
}

void tools_save_frame(const char* filename, const rgb8_t** I_bb, int w, int h) {
    char buffer[80];

    FILE* file;
    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */
    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i < h; i++)
        tools_write_PNM_row((uint8*)I_bb[i], w - 1, file);

    /* fermeture du fichier */
    fclose(file);
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

void tools_plot_bounding_box(rgb8_t** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8_t color) {
    for (int b = 0; b < border; b++) {
        ymin++;
        ymax--;
        xmin++;
        xmax--;
        for (int i = ymin; i <= ymax; i++) {
            img[i][xmin] = color;
            img[i][xmax] = color;
        }
        for (int j = xmin; j <= xmax; j++) {
            img[ymin][j] = color;
            img[ymax][j] = color;
        }
    }
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

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        tools_write_PNM_row((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix((rgb8**)img, 0, h - 1, 0, 2 * w - 1);
}

void tools_save_frame_ui32matrix(const char* filename, const uint32** I, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8_t** img = (rgb8_t**)rgb8matrix(0, h - 1, 0, w - 1);
    if (img == NULL)
        return;

    // (1,1) : filtrage surface
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = (I[i][j] == 0) ? 0 : 255;
            img[i][j].g = (I[i][j] == 0) ? 0 : 255;
            img[i][j].b = (I[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        tools_write_PNM_row((uint8*)img[i], w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix((rgb8**)img, 0, h - 1, 0, w - 1);
}

void tools_save_frame_ui8matrix(const char* filename, const uint8** I, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8_t** img = (rgb8_t**)rgb8matrix(0, h - 1, 0, w - 1);
    if (img == NULL)
        return;

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Failed opening '%s' file\n", filename);
        exit(-1);
    }

    /* enregistrement de l'image au format rpgm */
    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        tools_write_PNM_row((uint8*)img[i], w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix((rgb8**)img, 0, h - 1, 0, w - 1);
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

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
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

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
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

void tools_linear_2d_nrc_ui8matrix(const uint8_t* X, const int i0, const int i1, const int j0, const int j1, const int b, 
                         const uint8_t** Y) {
    Y[i0 - b] = X - (j0 - b);
    for (int i = i0 - b + 1; i <= i1 + b; i++)
        Y[i] = Y[i - 1] + ((j1 - j0) + 1 + 2 * b);
}

void tools_linear_2d_nrc_ui32matrix(const uint32_t* X, const int i0, const int i1, const int j0, const int j1, const int b, 
                         const uint32_t** Y) {
    Y[i0 - b] = X - (j0 - b);
    for (int i = i0 - b + 1; i <= i1 + b; i++)
        Y[i] = Y[i - 1] + ((j1 - j0) + 1 + 2 * b);
}

void tools_linear_2d_nrc_f32matrix(const float* X, const int i0, const int i1, const int j0, const int j1, const int b, 
                                   const float** Y) {
    Y[i0 - b] = X - (j0 - b);
    for (int i = i0 - b + 1; i <= i1 + b; i++)
        Y[i] = Y[i - 1] + ((j1 - j0) + 1 + 2 * b);
}
