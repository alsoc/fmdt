/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */ 

#include "string.h"
#include "stdio.h"

#ifdef OPENCV_LINK
#include <iostream>
#include <vector>
#include <tuple>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#endif

#include "DebugUtil.h"
#include "tools_visu.h"

enum Color_t g_obj_type_to_color[N_OBJ_TYPES];
char g_obj_type_to_string[N_OBJ_TYPES][64];
char g_obj_type_to_string_with_spaces[N_OBJ_TYPES][64];

void init_global_data() {
    g_obj_type_to_color[UNKNOWN] = UNKNOWN_COLOR;
    g_obj_type_to_color[STAR]    = STAR_COLOR;
    g_obj_type_to_color[METEOR]  = METEOR_COLOR;
    g_obj_type_to_color[NOISE]   = NOISE_COLOR;

    char str_unknown[64] = UNKNOWN_STR; sprintf(g_obj_type_to_string[UNKNOWN], "%s", str_unknown);
    char str_star   [64] = STAR_STR;    sprintf(g_obj_type_to_string[STAR   ], "%s", str_star   );
    char str_meteor [64] = METEOR_STR;  sprintf(g_obj_type_to_string[METEOR ], "%s", str_meteor );
    char str_noise  [64] = NOISE_STR;   sprintf(g_obj_type_to_string[NOISE  ], "%s", str_noise  );

    unsigned max = 0;
    for (int i = 0; i < N_OBJ_TYPES; i++)
        if (strlen(g_obj_type_to_string[i]) > max)
            max = strlen(g_obj_type_to_string[i]);

    for (int i = 0; i < N_OBJ_TYPES; i++) {
        int len = strlen(g_obj_type_to_string[i]);
        int diff = max - len;
        for (int c = len; c >= 0; c--)
            g_obj_type_to_string_with_spaces[i][diff +c] = g_obj_type_to_string[i][c];
        for (int c = 0; c < diff; c++)
            g_obj_type_to_string_with_spaces[i][c] = ' ';
    }
}

enum Obj_type string_to_obj_type(const char* string) {
    enum Obj_type obj = UNKNOWN;
    for (int i = 0; i < N_OBJ_TYPES; i++)
        if(!strcmp(string, g_obj_type_to_string[i])) {
            obj = (enum Obj_type)i;
            break;
        }
    return obj;
}

// ==============================================================================================================================
rgb8 get_color(enum Color_t color)
// ==============================================================================================================================
{
    rgb8 gray;     rgb8 green;     rgb8 red;     rgb8 blue;    rgb8 purple;    rgb8 orange;     rgb8 yellow;     rgb8 misc;
    gray.g = 175;  green.g = 255;  red.g = 000;  blue.g = 000; purple.g = 127; orange.r = 255;  yellow.g = 255;  misc.g = 255;
    gray.b = 175;  green.b = 000;  red.b = 000;  blue.b = 255; purple.b = 255; orange.g = 165;  yellow.b = 000;  misc.b = 153;
    gray.r = 175;  green.r = 000;  red.r = 255;  blue.r = 000; purple.r = 127; orange.b = 000;  yellow.r = 255;  misc.r = 153;

    switch (color) {
        case GRAY:   return gray;
        case GREEN:  return green;
        case RED:    return red;
        case BLUE:   return blue;
        case PURPLE: return purple;
        case ORANGE: return orange;
        case YELLOW: return yellow;
        case MISC:   return misc;
        default:     break;
    }
    return red;
}

#ifdef OPENCV_LINK // this is C++ code (because OpenCV API is C++ now)
void draw_legend_squares(rgb8** img, unsigned box_size, unsigned h_space, unsigned v_space, int validation) {
    //                     ymin      ymax      xmin      xmax     color
    std::vector<std::tuple<unsigned, unsigned, unsigned, unsigned, rgb8>> box_list;

    for (int i = 0; i < N_OBJ_TYPES; i++)
        box_list.push_back(std::make_tuple((i +1) * v_space + (i +0) * box_size, // ymin
                                           (i +1) * v_space + (i +1) * box_size, // ymax
                                           (  +1) * h_space + (  +0) * box_size, // xmin
                                           (  +1) * h_space + (  +1) * box_size, // xmax
                                           get_color(g_obj_type_to_color[i])));  // color

    if (validation)
        // add false positve meteor
        box_list.push_back(std::make_tuple((N_OBJ_TYPES +2) * v_space + (N_OBJ_TYPES +1) * box_size, // ymin
                                           (N_OBJ_TYPES +2) * v_space + (N_OBJ_TYPES +2) * box_size, // ymax
                                           (            +1) * h_space + (            +0) * box_size, // xmin
                                           (            +1) * h_space + (            +1) * box_size, // xmax
                                           get_color(RED)));                                         // color

    for (auto &box : box_list)
        plot_bounding_box(img, std::get<0>(box), std::get<1>(box), std::get<2>(box), std::get<3>(box), 2, std::get<4>(box));
}

void draw_legend_texts(cv::Mat &cv_img, unsigned box_size, unsigned h_space, unsigned v_space, int validation) {
    //                          color        pos         text
    std::vector<std::tuple<cv::Scalar, cv::Point, std::string>> txt_list;
    for (int i = 0; i < N_OBJ_TYPES; i++) {
        rgb8 color = get_color(g_obj_type_to_color[i]);
        unsigned x = 2 * h_space + box_size;
        unsigned y = ((i +1) * v_space + (i +1) * box_size) -2;
        txt_list.push_back(std::make_tuple(cv::Scalar(color.b, color.g, color.r),
                                           cv::Point(x, y),
                                           std::string(g_obj_type_to_string[i])));
    }

    if (validation) {
        // add false positve meteor
        rgb8 color = get_color(RED);
        unsigned x = 2 * h_space + box_size;
        unsigned y = ((N_OBJ_TYPES +2) * v_space + (N_OBJ_TYPES +2) * box_size) -2;
        txt_list.push_back(std::make_tuple(cv::Scalar(color.b, color.g, color.r),
                                           cv::Point(x, y),
                                           std::string("fp meteor")));
    }

    for (auto &txt : txt_list)
        cv::putText(cv_img,
                    std::get<2>(txt).c_str(), // text
                    std::get<1>(txt),         // position
                    cv::FONT_HERSHEY_DUPLEX,  // font type
                    0.7,                      // font size
                    std::get<0>(txt),         // color
                    1,                        // ?
                    cv::LINE_AA);             // ?
}

void draw_track_ids(cv::Mat &cv_img, const coordBB* listBB, const int nBB) {
    //                       x    y color        list of ids
    std::vector<std::tuple<int, int, rgb8, std::vector<int>>> list_of_ids_grouped_by_pos;
    for(int i = 0; i < nBB; i++) {
        int x = listBB[i].xmax+3;
        int y = (listBB[i].ymin)+((listBB[i].ymax-listBB[i].ymin)/2);

        bool found = false;
        for (auto &l : list_of_ids_grouped_by_pos) {
            rgb8 c = get_color(listBB[i].color);
            if (std::get<0>(l) == x && std::get<1>(l) == y &&
                std::get<2>(l).r == c.r && std::get<2>(l).g == c.g && std::get<2>(l).b == c.b) {
                std::get<3>(l).push_back(listBB[i].track_id);
                found = true;
            }
        }

        if (!found) {
            std::vector<int> v;
            v.push_back(listBB[i].track_id);
            list_of_ids_grouped_by_pos.push_back(std::make_tuple(x, y, get_color(listBB[i].color), v));
        }
    }

    for (auto id : list_of_ids_grouped_by_pos) {
        std::string txt = std::to_string(std::get<3>(id)[std::get<3>(id).size() -1]);
        for (int s = std::get<3>(id).size() -2; s >= 0; s--)
            txt += "," + std::to_string(std::get<3>(id)[s]);

        const int x = std::get<0>(id);
        const int y = std::get<1>(id);
        const rgb8 color = std::get<2>(id);

        // writing 'txt' over the image
        cv::Point org(x, y);
        cv::putText(cv_img, txt.c_str(), org,
                    cv::FONT_HERSHEY_DUPLEX, 0.7,
                    cv::Scalar(color.b, color.g, color.r), 1, cv::LINE_AA);
    }
}

void draw_text(rgb8** img, const int img_width, const int img_height, const coordBB* listBB, const int nBB, int validation, int show_ids) {
    unsigned box_size = 20, h_space = 10, v_space = 10;
    draw_legend_squares(img, box_size, h_space, v_space, validation);

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
            cv_img.at<cv::Vec3b>(i,j)[2] = img[i][j].r;
            cv_img.at<cv::Vec3b>(i,j)[1] = img[i][j].g;
            cv_img.at<cv::Vec3b>(i,j)[0] = img[i][j].b;
        }

    if (show_ids)
        draw_track_ids(cv_img, listBB, nBB);
    draw_legend_texts(cv_img, box_size, h_space, v_space, validation);

    // // debug: show image inside a window.
    // cv::imshow("Output", cv_img);
    // cv::waitKey(0);

    // convert back: 'cv::Mat' into 'img'
    for (int i = 0; i < img_height; i++) {
        for (int j = 0; j < img_width; j++) {
            img[i][j].r = cv_img.at<cv::Vec3b>(i,j)[2];
            img[i][j].g = cv_img.at<cv::Vec3b>(i,j)[1];
            img[i][j].b = cv_img.at<cv::Vec3b>(i,j)[0];
        }
    }
}
#endif

void convert_img_grayscale_to_rgb(const uint8** I, rgb8** I_bb, int i0, int i1, int j0, int j1) {
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            I_bb[i][j].r = I[i][j];
            I_bb[i][j].g = I[i][j];
            I_bb[i][j].b = I[i][j];
        }
    }
}

// ==============================================================================================================================
void draw_BB(rgb8** I_bb, const coordBB* listBB, int n_BB)
// ==============================================================================================================================
{
    for (int i = 0; i < n_BB; i++)
        plot_bounding_box(I_bb, listBB[i].ymin, listBB[i].ymax, listBB[i].xmin, listBB[i].xmax, 2, get_color(listBB[i].color));
}

// ==============================================================================================================================
void saveFrame(const char* filename, const rgb8** I_bb, int w, int h)
// ==============================================================================================================================
{
    char buffer[80];

    FILE *file;
    file = fopen(filename, "wb");
    if (file == NULL){
      char message[256] = "ouverture du fichier %s impossible dans saveFrame_listBB\n";
      nrerror(message);
    }

    /* enregistrement de l'image au format rpgm */
    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w-1), (int)(h-1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i < h; i++)
      WritePNMrow((uint8*)I_bb[i], w-1, file);

    /* fermeture du fichier */
    fclose(file);
}
