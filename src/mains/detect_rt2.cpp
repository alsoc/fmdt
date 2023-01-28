#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <nrc2.h>
#include <algorithm>

#include "fmdt/args.h"
#include "fmdt/macros.h"

#include "fmdt/aff3ct_wrapper/CCL_LSL/CCL_LSL.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_extractor.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_motion.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_magnitude.hpp"
#include "fmdt/aff3ct_wrapper/KNN_matcher/KNN_matcher.hpp"
#include "fmdt/aff3ct_wrapper/Threshold/Threshold.hpp"
#include "fmdt/aff3ct_wrapper/Tracking/Tracking.hpp"
#include "fmdt/aff3ct_wrapper/Video/Video2.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_ROI.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_KNN.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_motion.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_track.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_frame.hpp"

#define ENABLE_PIPELINE

int main(int argc, char** argv) {
    // default values
    int def_p_fra_start = 0;
    int def_p_fra_end = 0;
    int def_p_fra_skip = 0;
    int def_p_light_min = 55;
    int def_p_light_max = 80;
    int def_p_surface_min = 3;
    int def_p_surface_max = 1000;
    int def_p_k = 3;
    int def_p_max_dist = 10;
    float def_p_min_ratio_s = 0.125f;
    int def_p_r_extrapol = 10;
    int def_p_extrapol_order = 3;
    float def_p_angle_max = 20;
    int def_p_fra_star_min = 15;
    int def_p_fra_meteor_min = 3;
    int def_p_fra_meteor_max = 100;
    float def_p_diff_dev = 4.f;
    char* def_p_in_video = NULL;
    char* def_p_out_frames = NULL;
    char* def_p_out_bb = NULL;
    char* def_p_out_stats = NULL;
    char* def_p_out_mag = NULL;
    int def_p_ffmpeg_threads = 0;

    // Help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr,
                "  --in-video          Path to video file or to an images sequence                            [%s]\n",
                def_p_in_video ? def_p_in_video : "NULL");
        fprintf(stderr,
                "  --out-frames        Path to frames output folder                                           [%s]\n",
                def_p_out_frames ? def_p_out_frames : "NULL");
        fprintf(stderr,
                "  --out-bb            Path to the file containing the bounding boxes (frame by frame)        [%s]\n",
                def_p_out_bb ? def_p_out_bb : "NULL");
        fprintf(stderr,
                "  --out-stats         Path of the output statistics, only required for debugging purpose     [%s]\n",
                def_p_out_stats ? def_p_out_stats : "NULL");
        fprintf(stderr,
                "  --out-mag           Path to the file containing magnitudes of the tracked objects          [%s]\n",
                def_p_out_mag ? def_p_out_mag : "NULL");
        fprintf(stderr,
                "  --fra-start         Starting point of the video                                            [%d]\n",
                def_p_fra_start);
        fprintf(stderr,
                "  --fra-end           Ending point of the video                                              [%d]\n",
                def_p_fra_end);
        fprintf(stderr,
                "  --fra-skip          Number of skipped frames                                               [%d]\n",
                def_p_fra_skip);
        fprintf(stderr,
                "  --light-min         Low hysteresis threshold (grayscale [0;255])                           [%d]\n",
                def_p_light_min);
        fprintf(stderr,
                "  --light-max         High hysteresis threshold (grayscale [0;255])                          [%d]\n",
                def_p_light_max);
        fprintf(stderr,
                "  --surface-min       Maximum area of the CC                                                 [%d]\n",
                def_p_surface_min);
        fprintf(stderr,
                "  --surface-max       Minimum area of the CC                                                 [%d]\n",
                def_p_surface_max);
        fprintf(stderr,
                "  -k                  Maximum number of neighbors considered in k-NN algorithm               [%d]\n",
                def_p_k);
        fprintf(stderr,
                "  --max-dist          Maximum number of pixels between two images (in k-NN)                  [%d]\n",
                def_p_max_dist);
        fprintf(stderr,
                "  --min-ratio-s       Minimum surface ratio to match two CCs in k-NN                         [%f]\n",
                def_p_min_ratio_s);
        fprintf(stderr,
                "  --r-extrapol        Search radius for the next CC in case of extrapolation                 [%d]\n",
                def_p_r_extrapol);
        fprintf(stderr,
                "  --extrapol-order    Maximum number of frames to extrapolate objects (linear extrapolation) [%d]\n",
                def_p_extrapol_order);
        fprintf(stderr,
                "  --angle-max         Tracking angle max between two consecutive meteor moving points        [%f]\n",
                def_p_angle_max);
        fprintf(stderr,
                "  --fra-star-min      Minimum number of frames required to track a star                      [%d]\n",
                def_p_fra_star_min);
        fprintf(stderr,
                "  --fra-meteor-min    Minimum number of frames required to track a meteor                    [%d]\n",
                def_p_fra_meteor_min);
        fprintf(stderr,
                "  --fra-meteor-max    Maximum number of frames required to track a meteor                    [%d]\n",
                def_p_fra_meteor_max);
        fprintf(stderr,
                "  --diff-dev          Differential deviation factor for motion detection (motion error of        \n");
        fprintf(stderr,
                "                      one CC has to be superior to 'diff deviation' * 'standard deviation')  [%f]\n",
                def_p_diff_dev);
        fprintf(stderr,
                "  --track-all         Tracks all object types (star, meteor or noise)                            \n");
        fprintf(stderr,
                "  --task-stats        Display the statistics of tasks                                            \n");
        fprintf(stderr,
                "  --ffmpeg-threads    Select the number of threads to use to decode video input (in ffmpeg)  [%d]\n",
                def_p_ffmpeg_threads);
#ifdef OPENCV_LINK
        fprintf(stderr,
                "  --show-id           Show the ROI/CC ids on the ouptut frames                                   \n");
#endif
        fprintf(stderr,
                "  -h                  This help                                                                  \n");
        exit(1);
    }

    // parse arguments
    const int p_fra_start = args_find_int_min(argc, argv, "--fra-start", def_p_fra_start, 0);
    const int p_fra_end = args_find_int_min(argc, argv, "--fra-end", def_p_fra_end, 0);
    const int p_fra_skip = args_find_int_min(argc, argv, "--fra-skip", def_p_fra_skip, 0);
    const int p_light_min = args_find_int_min_max(argc, argv, "--light-min", def_p_light_min, 0, 255);
    const int p_light_max = args_find_int_min_max(argc, argv, "--light-max", def_p_light_max, 0, 255);
    const int p_surface_min = args_find_int_min(argc, argv, "--surface-min", def_p_surface_min, 0);
    const int p_surface_max = args_find_int_min(argc, argv, "--surface-max", def_p_surface_max, 0);
    const int p_k = args_find_int_min(argc, argv, "-k", def_p_k, 0);
    const int p_max_dist = args_find_int_min(argc, argv, "--max-dist", def_p_max_dist, 0);
    const float p_min_ratio_s = args_find_float_min_max(argc, argv, "--min-ratio-s", def_p_min_ratio_s, 0.f, 1.f);
    const int p_r_extrapol = args_find_int_min(argc, argv, "--r-extrapol", def_p_r_extrapol, 0);
    const int p_extrapol_order = args_find_int_min_max(argc, argv, "--extrapol-order", def_p_extrapol_order, 0, 255);
    const float p_angle_max = args_find_float_min_max(argc, argv, "--angle-max", def_p_angle_max, 0.f, 360.f);
    const int p_fra_star_min = args_find_int_min(argc, argv, "--fra-star-min", def_p_fra_star_min, 2);
    const int p_fra_meteor_min = args_find_int_min(argc, argv, "--fra-meteor-min", def_p_fra_meteor_min, 2);
    const int p_fra_meteor_max = args_find_int_min(argc, argv, "--fra-meteor-max", def_p_fra_meteor_max, 2);
    const float p_diff_dev = args_find_float_min(argc, argv, "--diff-dev", def_p_diff_dev, 0.f);
    const char* p_in_video = args_find_char(argc, argv, "--in-video", def_p_in_video);
    const char* p_out_frames = args_find_char(argc, argv, "--out-frames", def_p_out_frames);
    const char* p_out_bb = args_find_char(argc, argv, "--out-bb", def_p_out_bb);
    const char* p_out_stats = args_find_char(argc, argv, "--out-stats", def_p_out_stats);
    const char* p_out_mag = args_find_char(argc, argv, "--out-mag", def_p_out_mag);
    const int p_track_all = args_find(argc, argv, "--track-all");
    const int p_ffmpeg_threads = args_find_int_min(argc, argv, "--ffmpeg-threads", def_p_ffmpeg_threads, 0);
    const int p_task_stats = args_find(argc, argv, "--task-stats");
#ifdef OPENCV_LINK
    const int p_show_id = args_find(argc, argv, "--show-id");
#else
    const int p_show_id = 0;
#endif

    // heading display
    printf("#  ---------------------\n");
    printf("# |          ----*      |\n");
    printf("# | --* FMDT-DETECT --* |\n");
    printf("# |   -------*          |\n");
    printf("#  ---------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * in-video       = %s\n", p_in_video);
    printf("#  * out-bb         = %s\n", p_out_bb);
    printf("#  * out-frames     = %s\n", p_out_frames);
    printf("#  * out-stats      = %s\n", p_out_stats);
    printf("#  * out-mag        = %s\n", p_out_mag);
    printf("#  * fra-start      = %d\n", p_fra_start);
    printf("#  * fra-end        = %d\n", p_fra_end);
    printf("#  * fra-skip       = %d\n", p_fra_skip);
    printf("#  * light-min      = %d\n", p_light_min);
    printf("#  * light-max      = %d\n", p_light_max);
    printf("#  * surface-min    = %d\n", p_surface_min);
    printf("#  * surface-max    = %d\n", p_surface_max);
    printf("#  * k              = %d\n", p_k);
    printf("#  * max-dist       = %d\n", p_max_dist);
    printf("#  * min-ratio-s    = %1.3f\n", p_min_ratio_s);
    printf("#  * r-extrapol     = %d\n", p_r_extrapol);
    printf("#  * extrapol-order = %d\n", p_extrapol_order);
    printf("#  * angle-max      = %f\n", p_angle_max);
    printf("#  * fra-star-min   = %d\n", p_fra_star_min);
    printf("#  * fra-meteor-min = %d\n", p_fra_meteor_min);
    printf("#  * fra-meteor-max = %d\n", p_fra_meteor_max);
    printf("#  * diff-dev       = %4.2f\n", p_diff_dev);
    printf("#  * track-all      = %d\n", p_track_all);
    printf("#  * task-stats     = %d\n", p_task_stats);
    printf("#  * ffmpeg-threads = %d\n", p_ffmpeg_threads);
#ifdef OPENCV_LINK
    printf("#  * show-id        = %d\n", p_show_id);
#endif
#ifdef ENABLE_PIPELINE
    printf("#  * Runtime mode   = Pipeline\n");
#else
    printf("#  * Runtime mode   = Sequence\n");
#endif

    // arguments checking
    if (!p_in_video) {
        fprintf(stderr, "(EE) '--in-video' is missing\n");
        exit(1);
    }
    if (p_fra_meteor_max < p_fra_meteor_min) {
        fprintf(stderr, "(EE) '--fra-meteor-max' has to be bigger than '--fra-meteor-min'\n");
        exit(1);
    }
    if (p_fra_end && p_fra_end < p_fra_start) {
        fprintf(stderr, "(EE) '--fra-end' has to be higher than '--fra-start'\n");
        exit(1);
    }
    if (p_light_min > p_light_max) {
        fprintf(stderr, "(EE) '--light-max' has to be higher than '--light-min'\n");
        exit(1);
    }
    if (p_ffmpeg_threads < 0) {
        fprintf(stderr, "(EE) '--ffmpeg-threads' has to be bigger or equal to 0\n");
        exit(1);
    }
#ifdef OPENCV_LINK
    if (p_show_id && !p_out_frames)
        fprintf(stderr, "(WW) '--show-id' has to be combined with the '--out-frames' parameter\n");
#endif

    // -------------------------------- //
    // -- INITIALISATION GLOBAL DATA -- //
    // -------------------------------- //

    tracking_init_global_data();

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    // objects allocation
    const size_t b = 1; // image border
    Video2 video(p_in_video, p_fra_start, p_fra_end, p_fra_skip, 0, p_ffmpeg_threads, b);
    const size_t i0 = video.get_i0();
    const size_t i1 = video.get_i1();
    const size_t j0 = video.get_j0();
    const size_t j1 = video.get_j1();
    
    Threshold threshold_min0(i0, i1, j0, j1, b, p_light_min);
    threshold_min0.set_custom_name("Thr0<min>");
    Threshold threshold_max0(i0, i1, j0, j1, b, p_light_max);
    threshold_max0.set_custom_name("Thr0<max>");
    CCL_LSL lsl0(i0, i1, j0, j1, b);
    lsl0.set_custom_name("CCL_LSL0");
    Features_extractor extractor0(i0, i1, j0, j1, b, MAX_ROI_SIZE_BEFORE_SHRINK);
    extractor0.set_custom_name("Extractor0");
    Features_merger_CCL_HI merger0(i0, i1, j0, j1, b, p_surface_min, p_surface_max, MAX_ROI_SIZE_BEFORE_SHRINK, MAX_ROI_SIZE);
    merger0.set_custom_name("Merger0");
    Features_magnitude magnitude0(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    magnitude0.set_custom_name("Magnitude0");

    Threshold threshold_min1(i0, i1, j0, j1, b, p_light_min);
    threshold_min1.set_custom_name("Thr1<min>");
    Threshold threshold_max1(i0, i1, j0, j1, b, p_light_max);
    threshold_max1.set_custom_name("Thr1<max>");
    CCL_LSL lsl1(i0, i1, j0, j1, b);
    lsl1.set_custom_name("CCL_LSL1");
    Features_extractor extractor1(i0, i1, j0, j1, b, MAX_ROI_SIZE_BEFORE_SHRINK);
    extractor1.set_custom_name("Extractor1");
    Features_merger_CCL_HI merger1(i0, i1, j0, j1, b, p_surface_min, p_surface_max, MAX_ROI_SIZE_BEFORE_SHRINK, MAX_ROI_SIZE);
    merger1.set_custom_name("Merger1");
    Features_magnitude magnitude1(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    magnitude1.set_custom_name("Magnitude1");

    KNN_matcher matcher(p_k, p_max_dist, p_min_ratio_s, MAX_ROI_SIZE);
    Features_motion motion(MAX_ROI_SIZE);
    motion.set_custom_name("Motion");
    Tracking tracking(p_r_extrapol, p_angle_max, p_diff_dev, p_track_all, p_fra_star_min, p_fra_meteor_min,
                      p_fra_meteor_max, p_out_bb, p_out_mag, p_extrapol_order, p_min_ratio_s, MAX_ROI_SIZE);
    Logger_ROI log_ROI(p_out_stats ? p_out_stats : "", p_fra_start, p_fra_skip, MAX_ROI_SIZE, tracking.get_data());
    Logger_KNN log_KNN(p_out_stats ? p_out_stats : "", p_fra_start, MAX_ROI_SIZE);
    Logger_motion log_motion(p_out_stats ? p_out_stats : "", p_fra_start);
    log_motion.set_custom_name("Logger_motio");
    Logger_track log_track(p_out_stats ? p_out_stats : "", p_fra_start, tracking.get_data());
    std::unique_ptr<Logger_frame> log_frame;
    if (p_out_frames)
        log_frame.reset(new Logger_frame(p_out_frames, p_fra_start, p_show_id, i0, i1, j0, j1, b, MAX_ROI_SIZE));

    // ------------------- //
    // -- TASKS BINDING -- //
    // ------------------- //

    // Step 1 : seuillage low/high
    threshold_min0[thr::sck::apply::in_img] = video[vid2::sck::generate::out_img0];
    threshold_max0[thr::sck::apply::in_img] = video[vid2::sck::generate::out_img0];
    threshold_min1[thr::sck::apply::in_img] = video[vid2::sck::generate::out_img1];
    threshold_max1[thr::sck::apply::in_img] = video[vid2::sck::generate::out_img1];
    
    // Step 2 : ECC/ACC
    lsl0[ccl::sck::apply::in_img] = threshold_min0[thr::sck::apply::out_img];
    extractor0[ftr_ext::sck::extract::in_img] = lsl0[ccl::sck::apply::out_labels];
    extractor0[ftr_ext::sck::extract::in_n_ROI] = lsl0[ccl::sck::apply::out_n_ROI];

    lsl1[ccl::sck::apply::in_img] = threshold_min1[thr::sck::apply::out_img];
    extractor1[ftr_ext::sck::extract::in_img] = lsl1[ccl::sck::apply::out_labels];
    extractor1[ftr_ext::sck::extract::in_n_ROI] = lsl1[ccl::sck::apply::out_n_ROI];

    // Step 3 : seuillage hysteresis && filter surface
    merger0[ftr_mrg::sck::merge::in_labels] = lsl0[ccl::sck::apply::out_labels];
    merger0[ftr_mrg::sck::merge::in_img_HI] = threshold_max0[thr::sck::apply::out_img];
    merger0[ftr_mrg::sck::merge::in_ROI_id] = extractor0[ftr_ext::sck::extract::out_ROI_id];
    merger0[ftr_mrg::sck::merge::in_ROI_xmin] = extractor0[ftr_ext::sck::extract::out_ROI_xmin];
    merger0[ftr_mrg::sck::merge::in_ROI_xmax] = extractor0[ftr_ext::sck::extract::out_ROI_xmax];
    merger0[ftr_mrg::sck::merge::in_ROI_ymin] = extractor0[ftr_ext::sck::extract::out_ROI_ymin];
    merger0[ftr_mrg::sck::merge::in_ROI_ymax] = extractor0[ftr_ext::sck::extract::out_ROI_ymax];
    merger0[ftr_mrg::sck::merge::in_ROI_S] = extractor0[ftr_ext::sck::extract::out_ROI_S];
    merger0[ftr_mrg::sck::merge::in_ROI_Sx] = extractor0[ftr_ext::sck::extract::out_ROI_Sx];
    merger0[ftr_mrg::sck::merge::in_ROI_Sy] = extractor0[ftr_ext::sck::extract::out_ROI_Sy];
    merger0[ftr_mrg::sck::merge::in_ROI_x] = extractor0[ftr_ext::sck::extract::out_ROI_x];
    merger0[ftr_mrg::sck::merge::in_ROI_y] = extractor0[ftr_ext::sck::extract::out_ROI_y];
    merger0[ftr_mrg::sck::merge::in_n_ROI] = lsl0[ccl::sck::apply::out_n_ROI];

    merger1[ftr_mrg::sck::merge::in_labels] = lsl1[ccl::sck::apply::out_labels];
    merger1[ftr_mrg::sck::merge::in_img_HI] = threshold_max1[thr::sck::apply::out_img];
    merger1[ftr_mrg::sck::merge::in_ROI_id] = extractor1[ftr_ext::sck::extract::out_ROI_id];
    merger1[ftr_mrg::sck::merge::in_ROI_xmin] = extractor1[ftr_ext::sck::extract::out_ROI_xmin];
    merger1[ftr_mrg::sck::merge::in_ROI_xmax] = extractor1[ftr_ext::sck::extract::out_ROI_xmax];
    merger1[ftr_mrg::sck::merge::in_ROI_ymin] = extractor1[ftr_ext::sck::extract::out_ROI_ymin];
    merger1[ftr_mrg::sck::merge::in_ROI_ymax] = extractor1[ftr_ext::sck::extract::out_ROI_ymax];
    merger1[ftr_mrg::sck::merge::in_ROI_S] = extractor1[ftr_ext::sck::extract::out_ROI_S];
    merger1[ftr_mrg::sck::merge::in_ROI_Sx] = extractor1[ftr_ext::sck::extract::out_ROI_Sx];
    merger1[ftr_mrg::sck::merge::in_ROI_Sy] = extractor1[ftr_ext::sck::extract::out_ROI_Sy];
    merger1[ftr_mrg::sck::merge::in_ROI_x] = extractor1[ftr_ext::sck::extract::out_ROI_x];
    merger1[ftr_mrg::sck::merge::in_ROI_y] = extractor1[ftr_ext::sck::extract::out_ROI_y];
    merger1[ftr_mrg::sck::merge::in_n_ROI] = lsl1[ccl::sck::apply::out_n_ROI];

    // Step 3.5 : calcul de la magnitude pour chaque ROI
    magnitude0[ftr_mgn::sck::compute::in_img] = video[vid2::sck::generate::out_img0];
    magnitude0[ftr_mgn::sck::compute::in_labels] = merger0[ftr_mrg::sck::merge::out_labels];
    magnitude0[ftr_mgn::sck::compute::in_ROI_xmin] = merger0[ftr_mrg::sck::merge::out_ROI_xmin];
    magnitude0[ftr_mgn::sck::compute::in_ROI_xmax] = merger0[ftr_mrg::sck::merge::out_ROI_xmax];
    magnitude0[ftr_mgn::sck::compute::in_ROI_ymin] = merger0[ftr_mrg::sck::merge::out_ROI_ymin];
    magnitude0[ftr_mgn::sck::compute::in_ROI_ymax] = merger0[ftr_mrg::sck::merge::out_ROI_ymax];
    magnitude0[ftr_mgn::sck::compute::in_ROI_S] = merger0[ftr_mrg::sck::merge::out_ROI_S];
    magnitude0[ftr_mgn::sck::compute::in_n_ROI] = merger0[ftr_mrg::sck::merge::out_n_ROI];

    magnitude1[ftr_mgn::sck::compute::in_img] = video[vid2::sck::generate::out_img1];
    magnitude1[ftr_mgn::sck::compute::in_labels] = merger1[ftr_mrg::sck::merge::out_labels];
    magnitude1[ftr_mgn::sck::compute::in_ROI_xmin] = merger1[ftr_mrg::sck::merge::out_ROI_xmin];
    magnitude1[ftr_mgn::sck::compute::in_ROI_xmax] = merger1[ftr_mrg::sck::merge::out_ROI_xmax];
    magnitude1[ftr_mgn::sck::compute::in_ROI_ymin] = merger1[ftr_mrg::sck::merge::out_ROI_ymin];
    magnitude1[ftr_mgn::sck::compute::in_ROI_ymax] = merger1[ftr_mrg::sck::merge::out_ROI_ymax];
    magnitude1[ftr_mgn::sck::compute::in_ROI_S] = merger1[ftr_mrg::sck::merge::out_ROI_S];
    magnitude1[ftr_mgn::sck::compute::in_n_ROI] = merger1[ftr_mrg::sck::merge::out_n_ROI];

    // Step 4 : mise en correspondance
    matcher[knn::sck::match::in_ROI0_id] = merger0[ftr_mrg::sck::merge::out_ROI_id];
    matcher[knn::sck::match::in_ROI0_S] = merger0[ftr_mrg::sck::merge::out_ROI_S];
    matcher[knn::sck::match::in_ROI0_x] = merger0[ftr_mrg::sck::merge::out_ROI_x];
    matcher[knn::sck::match::in_ROI0_y] = merger0[ftr_mrg::sck::merge::out_ROI_y];
    matcher[knn::sck::match::in_n_ROI0] = merger0[ftr_mrg::sck::merge::out_n_ROI];
    matcher[knn::sck::match::in_ROI1_id] = merger1[ftr_mrg::sck::merge::out_ROI_id];
    matcher[knn::sck::match::in_ROI1_S] = merger1[ftr_mrg::sck::merge::out_ROI_S];
    matcher[knn::sck::match::in_ROI1_x] = merger1[ftr_mrg::sck::merge::out_ROI_x];
    matcher[knn::sck::match::in_ROI1_y] = merger1[ftr_mrg::sck::merge::out_ROI_y];
    matcher[knn::sck::match::in_n_ROI1] = merger1[ftr_mrg::sck::merge::out_n_ROI];

    // Step 5 : recalage
    motion[ftr_mtn::sck::compute::in_ROI0_x] = merger0[ftr_mrg::sck::merge::out_ROI_x];
    motion[ftr_mtn::sck::compute::in_ROI0_y] = merger0[ftr_mrg::sck::merge::out_ROI_y];
    motion[ftr_mtn::sck::compute::in_ROI1_x] = merger1[ftr_mrg::sck::merge::out_ROI_x];
    motion[ftr_mtn::sck::compute::in_ROI1_y] = merger1[ftr_mrg::sck::merge::out_ROI_y];
    motion[ftr_mtn::sck::compute::in_ROI1_prev_id] = matcher[knn::sck::match::out_ROI1_prev_id];
    motion[ftr_mtn::sck::compute::in_n_ROI1] = merger1[ftr_mrg::sck::merge::out_n_ROI];

    // Step 6 : tracking
    tracking[trk::sck::perform::in_frame] = video[vid2::sck::generate::out_frame];
    tracking[trk::sck::perform::in_ROI_id] = merger1[ftr_mrg::sck::merge::out_ROI_id];
    tracking[trk::sck::perform::in_ROI_xmin] = merger1[ftr_mrg::sck::merge::out_ROI_xmin];
    tracking[trk::sck::perform::in_ROI_xmax] = merger1[ftr_mrg::sck::merge::out_ROI_xmax];
    tracking[trk::sck::perform::in_ROI_ymin] = merger1[ftr_mrg::sck::merge::out_ROI_ymin];
    tracking[trk::sck::perform::in_ROI_ymax] = merger1[ftr_mrg::sck::merge::out_ROI_ymax];
    tracking[trk::sck::perform::in_ROI_S] = merger1[ftr_mrg::sck::merge::out_ROI_S];
    tracking[trk::sck::perform::in_ROI_x] = merger1[ftr_mrg::sck::merge::out_ROI_x];
    tracking[trk::sck::perform::in_ROI_y] = merger1[ftr_mrg::sck::merge::out_ROI_y];
    tracking[trk::sck::perform::in_ROI_error] = motion[ftr_mtn::sck::compute::out_ROI1_error];
    tracking[trk::sck::perform::in_ROI_prev_id] = matcher[knn::sck::match::out_ROI1_prev_id];
    tracking[trk::sck::perform::in_ROI_magnitude] = magnitude1[ftr_mgn::sck::compute::out_ROI_magnitude];
    tracking[trk::sck::perform::in_n_ROI] = merger1[ftr_mrg::sck::merge::out_n_ROI];
    tracking[trk::sck::perform::in_motion_est] = motion[ftr_mtn::sck::compute::out_motion_est2];

    if (p_out_frames) {
        (*log_frame)[lgr_fra::sck::write::in_labels] = merger1[ftr_mrg::sck::merge::out_labels];
        (*log_frame)[lgr_fra::sck::write::in_ROI_id] = merger1[ftr_mrg::sck::merge::out_ROI_id];
        (*log_frame)[lgr_fra::sck::write::in_ROI_xmax] = merger1[ftr_mrg::sck::merge::out_ROI_xmax];
        (*log_frame)[lgr_fra::sck::write::in_ROI_ymin] = merger1[ftr_mrg::sck::merge::out_ROI_ymin];
        (*log_frame)[lgr_fra::sck::write::in_ROI_ymax] = merger1[ftr_mrg::sck::merge::out_ROI_ymax];
        (*log_frame)[lgr_fra::sck::write::in_n_ROI] = merger1[ftr_mrg::sck::merge::out_n_ROI];
    }

    if (p_out_stats) {
        log_ROI[lgr_roi::sck::write::in_ROI0_id] = merger0[ftr_mrg::sck::merge::out_ROI_id];  
        log_ROI[lgr_roi::sck::write::in_ROI0_xmin] = merger0[ftr_mrg::sck::merge::out_ROI_xmin];  
        log_ROI[lgr_roi::sck::write::in_ROI0_xmax] = merger0[ftr_mrg::sck::merge::out_ROI_xmax];  
        log_ROI[lgr_roi::sck::write::in_ROI0_ymin] = merger0[ftr_mrg::sck::merge::out_ROI_ymin];  
        log_ROI[lgr_roi::sck::write::in_ROI0_ymax] = merger0[ftr_mrg::sck::merge::out_ROI_ymax];  
        log_ROI[lgr_roi::sck::write::in_ROI0_S] = merger0[ftr_mrg::sck::merge::out_ROI_S];  
        log_ROI[lgr_roi::sck::write::in_ROI0_Sx] = merger0[ftr_mrg::sck::merge::out_ROI_Sx];  
        log_ROI[lgr_roi::sck::write::in_ROI0_Sy] = merger0[ftr_mrg::sck::merge::out_ROI_Sy];  
        log_ROI[lgr_roi::sck::write::in_ROI0_x] = merger0[ftr_mrg::sck::merge::out_ROI_x];  
        log_ROI[lgr_roi::sck::write::in_ROI0_y] = merger0[ftr_mrg::sck::merge::out_ROI_y];
        log_ROI[lgr_roi::sck::write::in_ROI0_magnitude] = magnitude0[ftr_mgn::sck::compute::out_ROI_magnitude];
        log_ROI[lgr_roi::sck::write::in_n_ROI0] = merger0[ftr_mrg::sck::merge::out_n_ROI];  
        log_ROI[lgr_roi::sck::write::in_ROI1_id] = merger1[ftr_mrg::sck::merge::out_ROI_id];
        log_ROI[lgr_roi::sck::write::in_ROI1_xmin] = merger1[ftr_mrg::sck::merge::out_ROI_xmin];
        log_ROI[lgr_roi::sck::write::in_ROI1_xmax] = merger1[ftr_mrg::sck::merge::out_ROI_xmax];
        log_ROI[lgr_roi::sck::write::in_ROI1_ymin] = merger1[ftr_mrg::sck::merge::out_ROI_ymin];
        log_ROI[lgr_roi::sck::write::in_ROI1_ymax] = merger1[ftr_mrg::sck::merge::out_ROI_ymax];
        log_ROI[lgr_roi::sck::write::in_ROI1_S] = merger1[ftr_mrg::sck::merge::out_ROI_S];
        log_ROI[lgr_roi::sck::write::in_ROI1_Sx] = merger1[ftr_mrg::sck::merge::out_ROI_Sx];
        log_ROI[lgr_roi::sck::write::in_ROI1_Sy] = merger1[ftr_mrg::sck::merge::out_ROI_Sy];
        log_ROI[lgr_roi::sck::write::in_ROI1_x] = merger1[ftr_mrg::sck::merge::out_ROI_x];
        log_ROI[lgr_roi::sck::write::in_ROI1_y] = merger1[ftr_mrg::sck::merge::out_ROI_y];
        log_ROI[lgr_roi::sck::write::in_ROI1_magnitude] = magnitude1[ftr_mgn::sck::compute::out_ROI_magnitude];
        log_ROI[lgr_roi::sck::write::in_n_ROI1] = merger1[ftr_mrg::sck::merge::out_n_ROI];
        log_ROI[lgr_roi::sck::write::in_frame] = video[vid2::sck::generate::out_frame];

        log_KNN[lgr_knn::sck::write::in_data_nearest] = matcher[knn::sck::match::out_data_nearest];
        log_KNN[lgr_knn::sck::write::in_data_distances] = matcher[knn::sck::match::out_data_distances];
        log_KNN[lgr_knn::sck::write::in_ROI0_id] = merger0[ftr_mrg::sck::merge::out_ROI_id];
        log_KNN[lgr_knn::sck::write::in_ROI0_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
        log_KNN[lgr_knn::sck::write::in_n_ROI0] = merger0[ftr_mrg::sck::merge::out_n_ROI];
        log_KNN[lgr_knn::sck::write::in_ROI1_dx] = motion[ftr_mtn::sck::compute::out_ROI1_dx];
        log_KNN[lgr_knn::sck::write::in_ROI1_dy] = motion[ftr_mtn::sck::compute::out_ROI1_dy];
        log_KNN[lgr_knn::sck::write::in_ROI1_error] = motion[ftr_mtn::sck::compute::out_ROI1_error];
        log_KNN[lgr_knn::sck::write::in_ROI1_is_moving] = motion[ftr_mtn::sck::compute::out_ROI1_is_moving];
        log_KNN[lgr_knn::sck::write::in_n_ROI1] = merger1[ftr_mrg::sck::merge::out_n_ROI];
        log_KNN[lgr_knn::sck::write::in_frame] = video[vid2::sck::generate::out_frame];

        log_motion[lgr_mtn::sck::write::in_motion_est1] = motion[ftr_mtn::sck::compute::out_motion_est1];
        log_motion[lgr_mtn::sck::write::in_motion_est2] = motion[ftr_mtn::sck::compute::out_motion_est2];
        log_motion[lgr_mtn::sck::write::in_frame] = video[vid2::sck::generate::out_frame];

        log_track[lgr_trk::sck::write::in_frame] = video[vid2::sck::generate::out_frame];
    }

    // --------------------- //
    // -- CREATE SEQUENCE -- //
    // --------------------- //

#ifdef ENABLE_PIPELINE
    // pipeline definition with separation stages
    std::vector<std::tuple<std::vector<aff3ct::runtime::Task*>,
                           std::vector<aff3ct::runtime::Task*>,
                           std::vector<aff3ct::runtime::Task*>>> sep_stages =
    { // pipeline stage 0
      std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                      std::vector<aff3ct::runtime::Task*>>(
        { &video[vid2::tsk::generate],},
        { &video[vid2::tsk::generate],},
        { /* no exclusions in this stage */ } ),
      // pipeline stage 1
      std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                      std::vector<aff3ct::runtime::Task*>>(
        { &threshold_min0[thr::tsk::apply], &threshold_max0[thr::tsk::apply],  &threshold_min1[thr::tsk::apply], 
          &threshold_max1[thr::tsk::apply], &magnitude0[ftr_mgn::tsk::compute], &magnitude1[ftr_mgn::tsk::compute] },
        { &merger0[ftr_mrg::tsk::merge],&merger1[ftr_mrg::tsk::merge], },
        { } ),
      // pipeline stage 2
      std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                      std::vector<aff3ct::runtime::Task*>>(
        { 
          &matcher[knn::tsk::match],
          &motion[ftr_mtn::tsk::compute],
          &tracking[trk::tsk::perform],
          },
        { },
        { /* no exclusions in this stage */ } ),
    };

    if (p_out_stats) {
        std::get<0>(sep_stages[2]).push_back(&log_ROI[lgr_roi::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_KNN[lgr_knn::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_motion[lgr_mtn::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_track[lgr_trk::tsk::write]);
    }

    if (p_out_frames) {
        std::get<0>(sep_stages[2]).push_back(&(*log_frame)[lgr_fra::tsk::write]);
    }

    aff3ct::runtime::Pipeline sequence_or_pipeline({ &video[vid2::tsk::generate] }, // first task of the sequence
                                                   sep_stages,
                                                   {
                                                     1, // number of threads in the stage 0
                                                     4, // number of threads in the stage 1
                                                     1, // number of threads in the stage 2
                                                   }, {
                                                     16, // synchronization buffer size between stages 0 and 1
                                                     16, // synchronization buffer size between stages 1 and 2
                                                   }, {
                                                     false, // type of waiting between stages 0 and 1 (true = active, false = passive)
                                                     false, // type of waiting between stages 1 and 2 (true = active, false = passive)
                                                   });
#else
    aff3ct::runtime::Sequence sequence_or_pipeline(video[vid2::tsk::generate], 1);
#endif

    // configuration of the sequence tasks
    for (auto& mod : sequence_or_pipeline.get_modules<aff3ct::module::Module>(false))
        for (auto& tsk : mod->tasks) {
            tsk->set_debug(false); // disable the debug mode
            tsk->set_debug_limit(16); // display only the 16 first bits if the debug mode is enabled
            tsk->set_stats(p_task_stats); // enable the statistics
            // enable the fast mode (= disable the useless verifs in the tasks) if there is no debug and stats modes
            if (!tsk->is_debug() && !tsk->is_stats())
                tsk->set_fast(true);
        }

    std::ofstream fs("runtime_dag.dot");
    sequence_or_pipeline.export_dot(fs);

    // ---------------------- //
    // -- EXECUTE SEQUENCE -- //
    // ---------------------- //

    unsigned n_frames = 0;
    std::function<bool(const std::vector<const int*>&)> stop_condition =
        [&tracking, &n_frames] (const std::vector<const int*>& statuses) {
            fprintf(stderr, "(II) Frame n°%4u", n_frames);
            unsigned n_stars = 0, n_meteors = 0, n_noise = 0;
            size_t n_tracks = tracking_count_objects(tracking.get_data()->tracks, &n_stars, &n_meteors, &n_noise);
            fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\r", n_meteors,
                    n_stars, n_noise, (unsigned long)n_tracks);
            fflush(stderr);
            n_frames++;
            return false;
        };

    printf("# The program is running...\n");

#ifdef ENABLE_PIPELINE
    sequence_or_pipeline.exec({
        stop_condition,                                                   // stop condition stage 0
        [] (const std::vector<const int*>& statuses) { return false; },   // stop condition stage 1
        [] (const std::vector<const int*>& statuses) { return false; }}); // stop condition stage 2
#else
    sequence_or_pipeline.exec(stop_condition);
#endif

    // ------------------- //
    // -- PRINT RESULTS -- //
    // ------------------- //

    fprintf(stderr, "\n");
    if (p_out_bb) {
        FILE* f = fopen(p_out_bb, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_out_bb);
            exit(1);
        }
        tracking_BB_array_write(f, tracking.get_BB_array(), tracking.get_data()->tracks);
        fclose(f);
    }

    if (p_out_mag) {
        FILE* f = fopen(p_out_mag, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_out_bb);
            exit(1);
        }
        tracking_track_array_magnitude_write(f, tracking.get_data()->tracks);
        fclose(f);
    }
    tracking_track_array_write(stdout, tracking.get_data()->tracks);

    unsigned n_stars = 0, n_meteors = 0, n_noise = 0;
    size_t real_n_tracks = tracking_count_objects(tracking.get_data()->tracks, &n_stars, &n_meteors, &n_noise);
    printf("# Tracks statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames -1);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, (unsigned long)real_n_tracks);

    // display the statistics of the tasks (if enabled)
#ifdef ENABLE_PIPELINE
    auto stages = sequence_or_pipeline.get_stages();
    for (size_t s = 0; s < stages.size(); s++)
    {
        const int n_threads = stages[s]->get_n_threads();
        std::cout << "#" << std::endl << "# Pipeline stage " << s << " (" << n_threads << " thread(s)): " << std::endl;
        aff3ct::tools::Stats::show(stages[s]->get_tasks_per_types(), true, false);
    }
#else
    std::cout << "#" << std::endl;
    aff3ct::tools::Stats::show(sequence_or_pipeline.get_tasks_per_types(), true, false);
#endif

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
