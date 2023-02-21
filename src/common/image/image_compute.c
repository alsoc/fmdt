#include <assert.h>
#include <string.h>
#include <stdlib.h>

#ifdef FMDT_OPENCV_LINK
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

#include "fmdt/tracking/tracking_global.h"
#include "fmdt/image/image_compute.h"

#define DELTA_BB 5 // extra pixel size for bounding boxes

rgb8_t image_get_color(enum color_e color) {
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
    case COLOR_GRAY:
        return gray;
    case COLOR_GREEN:
        return green;
    case COLOR_RED:
        return red;
    case COLOR_BLUE:
        return blue;
    case COLOR_PURPLE:
        return purple;
    case COLOR_ORANGE:
        return orange;
    case COLOR_YELLOW:
        return yellow;
    case COLOR_MISC:
        return misc;
    default:
        break;
    }
    return red;
}

void image_plot_bounding_box(rgb8_t** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8_t color,
                             int is_dashed) {
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
                img[i][xmin] = color;
                img[i][xmax] = color;
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
                img[ymin][j] = color;
                img[ymax][j] = color;
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

#ifdef FMDT_OPENCV_LINK // this is C++ code (because OpenCV API is C++ now)
void image_draw_legend_squares(rgb8_t** img, unsigned box_size, unsigned h_space, unsigned v_space, int validation) {
    //                     ymin      ymax      xmin      xmax      color
    std::vector<std::tuple<unsigned, unsigned, unsigned, unsigned, rgb8_t>> box_list;

    for (int i = 0; i < N_OBJECTS; i++)
        box_list.push_back(std::make_tuple((i + 1) * v_space + (i + 0) * box_size, // ymin
                                           (i + 1) * v_space + (i + 1) * box_size, // ymax
                                           (+1) * h_space + (+0) * box_size,       // xmin
                                           (+1) * h_space + (+1) * box_size,       // xmax
                                           image_get_color(g_obj_to_color[i])));   // color

    if (validation)
        // add false positive meteor
        box_list.push_back(std::make_tuple((N_OBJECTS + 2) * v_space + (N_OBJECTS + 1) * box_size, // ymin
                                           (N_OBJECTS + 2) * v_space + (N_OBJECTS + 2) * box_size, // ymax
                                           (+1) * h_space + (+0) * box_size,                       // xmin
                                           (+1) * h_space + (+1) * box_size,                       // xmax
                                           image_get_color(COLOR_RED)));                           // color

    for (auto& box : box_list)
        image_plot_bounding_box(img, std::get<0>(box), std::get<1>(box), std::get<2>(box), std::get<3>(box), 2,
                                std::get<4>(box), /* is_dashed = */ 0);
}

void image_draw_legend_text(cv::Mat& cv_img, unsigned box_size, unsigned h_space, unsigned v_space, int validation) {
    //                          color        pos         text
    std::vector<std::tuple<cv::Scalar, cv::Point, std::string>> txt_list;
    for (int i = 0; i < N_OBJECTS; i++) {
        rgb8_t color = image_get_color(g_obj_to_color[i]);
        unsigned x = 2 * h_space + box_size;
        unsigned y = ((i + 1) * v_space + (i + 1) * box_size) - 2;
        txt_list.push_back(std::make_tuple(cv::Scalar(color.r, color.g, color.b), cv::Point(x, y),
                                           std::string(g_obj_to_string[i])));
    }

    if (validation) {
        // add false positve meteor
        rgb8_t color = image_get_color(COLOR_RED);
        unsigned x = 2 * h_space + box_size;
        unsigned y = ((N_OBJECTS + 2) * v_space + (N_OBJECTS + 2) * box_size) - 2;
        txt_list.push_back(
            std::make_tuple(cv::Scalar(color.r, color.g, color.b), cv::Point(x, y), std::string("fp meteor")));
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

void image_draw_track_id(cv::Mat& cv_img, const BB_t* BBs, const enum color_e* BBs_color, const int nBB) {
    //                       x    y   color       list of ids
    std::vector<std::tuple<int, int, rgb8_t, std::vector<int>>> list_of_ids_grouped_by_pos;
    for (int i = 0; i < nBB; i++) {
        int ymin = BBs[i].bb_y - (BBs[i].ry + DELTA_BB);
        int ymax = BBs[i].bb_y + (BBs[i].ry + DELTA_BB);
        // int xmin = BBs[i].bb_x - (BBs[i].rx + DELTA_BB);
        int xmax = BBs[i].bb_x + (BBs[i].rx + DELTA_BB);

        int x = xmax + 3;
        int y = (ymin) + ((ymax - ymin) / 2);

        bool found = false;
        for (auto& l : list_of_ids_grouped_by_pos) {
            rgb8_t c = image_get_color(BBs_color[i]);
            if (std::get<0>(l) == x && std::get<1>(l) == y && std::get<2>(l).r == c.r && std::get<2>(l).g == c.g &&
                std::get<2>(l).b == c.b) {
                std::get<3>(l).push_back(BBs[i].track_id);
                found = true;
            }
        }

        if (!found) {
            std::vector<int> v;
            v.push_back(BBs[i].track_id);
            list_of_ids_grouped_by_pos.push_back(std::make_tuple(x, y, image_get_color(BBs_color[i]), v));
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
        cv::putText(cv_img, txt.c_str(), org, cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(color.r, color.g, color.b), 1,
                    cv::LINE_AA);
    }
}

void image_draw_text(img_data_t* img_data, const BB_t* BBs, const enum color_e* BBs_color, const int nBB,
                     int validation, int show_id) {
    unsigned box_size = 20, h_space = 10, v_space = 10;
    image_draw_legend_squares(image_color_get_pixels_2d(img_data), box_size, h_space, v_space, validation);
    cv::Mat* cv_mat = (cv::Mat*)img_data->pixels;
    if (show_id)
        image_draw_track_id(*cv_mat, BBs, BBs_color, nBB);
    image_draw_legend_text(*cv_mat, box_size, h_space, v_space, validation);
}

void _image_draw_RoIs_id(cv::Mat& cv_img, const uint32_t* RoIs_id, const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin,
                         const uint32_t* RoIs_ymax, const size_t n_RoIs) {
    //                       x    y  list of ids
    std::vector<std::tuple<int, int, std::vector<int>>> list_of_ids_grouped_by_pos;
    for (size_t i = 0; i < n_RoIs; i++) {
        int x = RoIs_xmax[i] + 3;
        int y = RoIs_ymin[i] + (RoIs_ymax[i] - RoIs_ymin[i]) / 2;

        bool found = false;
        for (auto& l : list_of_ids_grouped_by_pos) {
            if (std::get<0>(l) == x && std::get<1>(l) == y) {
                std::get<2>(l).push_back(RoIs_id[i]);
                found = true;
            }
        }

        if (!found) {
            std::vector<int> v;
            v.push_back(RoIs_id[i]);
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

void image_draw_BBs(rgb8_t** I_bb, const BB_t* BBs, const enum color_e* BBs_color, int n_BBs, int w, int h) {
    int border = 2;
    for (int i = 0; i < n_BBs; i++) {
        int ymin = BBs[i].bb_y - (BBs[i].ry + DELTA_BB);
        int ymax = BBs[i].bb_y + (BBs[i].ry + DELTA_BB);
        int xmin = BBs[i].bb_x - (BBs[i].rx + DELTA_BB);
        int xmax = BBs[i].bb_x + (BBs[i].rx + DELTA_BB);

        int ymin_fix = CLAMP(ymin, border + 1, h - (border + 2));
        int ymax_fix = CLAMP(ymax, border + 1, h - (border + 2));
        int xmin_fix = CLAMP(xmin, border + 1, w - (border + 2));
        int xmax_fix = CLAMP(xmax, border + 1, w - (border + 2));

        image_plot_bounding_box(I_bb, ymin_fix, ymax_fix, xmin_fix, xmax_fix, border, image_get_color(BBs_color[i]),
                                BBs[i].is_extrapolated);
    }
}

rgb8_t** image_color_load(const char* filename, long* i0, long* i1, long* j0, long* j1) {
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

img_data_t* image_gs_alloc(const size_t img_width, const size_t img_height) {
    img_data_t* img_data = (img_data_t*)malloc(sizeof(img_data_t));
    img_data->width = img_width;
    img_data->height = img_height;
#ifdef FMDT_OPENCV_LINK
    img_data->pixels = (void*) new cv::Mat(img_data->height, img_data->width, CV_8U, cv::Scalar(255));
    uint8_t** container_2d = (uint8_t**) malloc(sizeof(uint8_t*) * img_data->height);
    tools_linear_2d_nrc_ui8matrix((const uint8_t*)image_gs_get_pixels(img_data), 0, img_data->height - 1,
                                  0, img_data->width - 1, (const uint8_t**)container_2d);
    img_data->container_2d = (void*)container_2d;
#else
    img_data->pixels = (void*) ui8matrix(0, img_data->height -1, 0, img_data->width -1);
    img_data->container_2d = img_data->pixels;
#endif
    return img_data;
}

void _image_gs_draw_labels(img_data_t* img_data, const uint32_t** labels, const uint32_t* RoIs_id,
                           const uint32_t* RoIs_xmin, const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin,
                           const uint32_t* RoIs_ymax, const size_t n_RoIs, const uint8_t show_id) {
#ifdef FMDT_OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    // convert labels to black & white image: white if there is a CC, black otherwise
    // for (size_t i = 0; i < (size_t)pixels->rows; i++)
    //     for (size_t j = 0; j < (size_t)pixels->cols; j++)
    //         pixels->at<uint8_t>(i, j) = (labels[i][j] == 0) ? 0 : 255;

    for (size_t i = 0; i < (size_t)pixels->rows; i++)
        for (size_t j = 0; j < (size_t)pixels->cols; j++)
            pixels->at<uint8_t>(i, j) = 0;

    for (size_t i = 0; i < n_RoIs; i++) {
        uint32_t id = RoIs_id[i];
        uint32_t x0 = RoIs_ymin[i];
        uint32_t x1 = RoIs_ymax[i];
        uint32_t y0 = RoIs_xmin[i];
        uint32_t y1 = RoIs_xmax[i];
        for (uint32_t k = x0; k <= x1; k++)
            for (uint32_t l = y0; l <= y1; l++)
                if (labels[k][l] == id)
                    pixels->at<uint8_t>(k, l) = 255;
    }

    if (show_id)
        _image_draw_RoIs_id(*pixels, RoIs_id, RoIs_xmax, RoIs_ymin, RoIs_ymax, n_RoIs);
#else
    uint8_t** pixels = (uint8_t**)img_data->pixels;
    // convert labels to black & white image: white if there is a CC, black otherwise
    // for (size_t i = 0; i < img_data->height; i++)
    //     for (size_t j = 0; j < img_data->width; j++)
    //         pixels[i][j] = (labels[i][j] == 0) ? 0 : 255;

    for (size_t i = 0; i < img_data->height; i++)
        memset(pixels[i], 0, img_data->width * sizeof(uint8_t));

    for (size_t i = 0; i < n_RoIs; i++) {
        uint32_t id = RoIs_id[i];
        uint32_t x0 = RoIs_ymin[i];
        uint32_t x1 = RoIs_ymax[i];
        uint32_t y0 = RoIs_xmin[i];
        uint32_t y1 = RoIs_xmax[i];
        for (uint32_t k = x0; k <= x1; k++)
            for (uint32_t l = y0; l <= y1; l++)
                if (labels[k][l] == id)
                    pixels[k][l] = 255;
    }
#endif
}

void image_gs_draw_labels(img_data_t* img_data, const uint32_t** labels, const RoIs_basic_t* RoIs_basic,
                          const uint8_t show_id) {
    _image_gs_draw_labels(img_data, labels, RoIs_basic->id, RoIs_basic->xmin, RoIs_basic->xmax, RoIs_basic->ymin,
                          RoIs_basic->ymax, *RoIs_basic->_size, show_id);
}

uint8_t* image_gs_get_pixels(img_data_t* img_data) {
    uint8_t* raw_data = NULL;
#ifdef FMDT_OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    raw_data = (uint8_t*)pixels->data;
#else
    uint8_t** pixels = (uint8_t**)img_data->pixels;
    raw_data = pixels[0];
#endif
    return raw_data;
}

uint8_t** image_gs_get_pixels_2d(img_data_t* img_data) {
    return (uint8_t**)img_data->container_2d;
}

void image_gs_free(img_data_t* img_data) {
#ifdef FMDT_OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    delete pixels;
    free((uint8_t**)img_data->container_2d);
#else
    uint8_t** pixels = (uint8_t**)img_data->pixels;
    free_ui8matrix(pixels, 0, img_data->height -1, 0, img_data->width -1);
#endif
    free(img_data);
}

img_data_t* image_color_alloc(const size_t img_width, const size_t img_height) {
    img_data_t* img_data = (img_data_t*)malloc(sizeof(img_data_t));
    img_data->width = img_width;
    img_data->height = img_height;
#ifdef FMDT_OPENCV_LINK
    img_data->pixels = (void*) new cv::Mat(img_data->height, img_data->width, CV_8UC3, cv::Scalar(255, 255, 255));
    rgb8_t** container_2d = (rgb8_t**) malloc(sizeof(rgb8_t*) * img_data->height);
    tools_linear_2d_nrc_rgb8matrix((const rgb8_t*)image_color_get_pixels(img_data), 0, img_data->height - 1, 0,
                                   img_data->width - 1, (const rgb8_t**)container_2d);
    img_data->container_2d = (void*)container_2d;
#else
    img_data->pixels = (void*)rgb8matrix(0, img_data->height -1, 0, img_data->width -1);
    img_data->container_2d = img_data->pixels;
#endif
    return img_data;
}

void image_color_draw_BBs(img_data_t* img_data, const uint8_t** img, const BB_t* BBs,
                          const enum color_e* BBs_color, const size_t n_BBs, const uint8_t show_id,
                          const uint8_t is_gt) {
#ifdef FMDT_OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    for (size_t i = 0; i < (size_t)pixels->rows; i++) {
        for (size_t j = 0; j < (size_t)pixels->cols; j++) {
            pixels->at<cv::Vec3b>(i, j)[2] = img[i][j];
            pixels->at<cv::Vec3b>(i, j)[1] = img[i][j];
            pixels->at<cv::Vec3b>(i, j)[0] = img[i][j];
        }
    }
#else
    rgb8_t** pixels = (rgb8_t**)img_data->pixels;
    for (size_t i = 0; i < img_data->height; i++) {
        for (size_t j = 0; j < img_data->width; j++) {
            pixels[i][j].r = img[i][j];
            pixels[i][j].g = img[i][j];
            pixels[i][j].b = img[i][j];
        }
    }
#endif
    image_draw_BBs(image_color_get_pixels_2d(img_data), BBs, BBs_color, n_BBs, img_data->width,
                   img_data->height);
#ifdef FMDT_OPENCV_LINK
    image_draw_text(img_data, BBs, BBs_color, n_BBs, is_gt, show_id);
#endif
}

rgb8_t* image_color_get_pixels(img_data_t* img_data) {
    rgb8_t* raw_data = NULL;
#ifdef FMDT_OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    raw_data = (rgb8_t*)pixels->data;
#else
    rgb8_t** pixels = (rgb8_t**)img_data->pixels;
    raw_data = pixels[0];
#endif
    return raw_data;
}

rgb8_t** image_color_get_pixels_2d(img_data_t* img_data) {
    return (rgb8_t**)img_data->container_2d;
}

void image_color_free(img_data_t* img_data) {
#ifdef FMDT_OPENCV_LINK
    cv::Mat* pixels = (cv::Mat*)img_data->pixels;
    delete pixels;
    free((rgb8_t**)img_data->container_2d);
#else
    rgb8_t** pixels = (rgb8_t**)img_data->pixels;
    free_rgb8matrix((rgb8**)pixels, 0, img_data->height -1, 0, img_data->width -1);
#endif
    free(img_data);
}
