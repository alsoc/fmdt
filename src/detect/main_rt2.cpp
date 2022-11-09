#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <nrc2.h>
#include <algorithm>

#include "fmdt/args.h"
#include "fmdt/defines.h"
#include "fmdt/CCL.h"
#include "fmdt/tools.h"
#include "fmdt/features.h"
#include "fmdt/KPPV.h"
#include "fmdt/threshold.h"
#include "fmdt/tracking.h"
#include "fmdt/video.h"
#include "fmdt/macros.h"

#include "fmdt/CCL_LSL/CCL_LSL.hpp"
#include "fmdt/Features/Features_extractor.hpp"
#include "fmdt/Features/Features_merger.hpp"
#include "fmdt/Features/Features_motion.hpp"
#include "fmdt/KNN_matcher/KNN_matcher.hpp"
#include "fmdt/Threshold/Threshold.hpp"
#include "fmdt/Tracking/Tracking.hpp"
#include "fmdt/Video/Video2.hpp"
#include "fmdt/Logger/Logger_ROI.hpp"
#include "fmdt/Logger/Logger_KNN.hpp"
#include "fmdt/Logger/Logger_motion.hpp"
#include "fmdt/Logger/Logger_track.hpp"
#include "fmdt/Logger/Logger_frame.hpp"

int main(int argc, char** argv) {
    // default values
    int def_p_fra_start = 0;
    int def_p_fra_end = MAX_N_FRAMES;
    int def_p_fra_skip = 0;
    int def_p_light_min = 55;
    int def_p_light_max = 80;
    int def_p_surface_min = 3;
    int def_p_surface_max = 1000;
    int def_p_k = 3;
    int def_p_r_extrapol = 5;
    float def_p_angle_max = 20;
    int def_p_fra_star_min = 15;
    int def_p_fra_meteor_min = 3;
    int def_p_fra_meteor_max = 100;
    float def_p_diff_dev = 4.f;
    char* def_p_in_video = NULL;
    char* def_p_out_frames = NULL;
    char* def_p_out_bb = NULL;
    char* def_p_out_stats = NULL;

    // Help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr,
                "  --in-video          Path to video file                                                     [%s]\n",
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
                "  -k                  Number of neighbours                                                   [%d]\n",
                def_p_k);
        fprintf(stderr,
                "  --r-extrapol        Search radius for the next CC in case of extrapolation                 [%d]\n",
                def_p_r_extrapol);
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
                "  -h                  This help                                                                  \n");
        exit(1);
    }

    // Parsing Arguments
    const int p_fra_start = args_find_int(argc, argv, "--fra-start", def_p_fra_start);
    const int p_fra_end = args_find_int(argc, argv, "--fra-end", def_p_fra_end);
    const int p_fra_skip = args_find_int(argc, argv, "--fra-skip", def_p_fra_skip);
    const int p_light_min = args_find_int(argc, argv, "--light-min", def_p_light_min);
    const int p_light_max = args_find_int(argc, argv, "--light-max", def_p_light_max);
    const int p_surface_min = args_find_int(argc, argv, "--surface-min", def_p_surface_min);
    const int p_surface_max = args_find_int(argc, argv, "--surface-max", def_p_surface_max);
    const int p_k = args_find_int(argc, argv, "-k", def_p_k);
    const int p_r_extrapol = args_find_int(argc, argv, "--r-extrapol", def_p_r_extrapol);
    const float p_angle_max = args_find_float(argc, argv, "--angle-max", def_p_angle_max);
    const int p_fra_star_min = args_find_int(argc, argv, "--fra-star-min", def_p_fra_star_min);
    const int p_fra_meteor_min = args_find_int(argc, argv, "--fra-meteor-min", def_p_fra_meteor_min);
    const int p_fra_meteor_max = args_find_int(argc, argv, "--fra-meteor-max", def_p_fra_meteor_max);
    const float p_diff_dev = args_find_float(argc, argv, "--diff-dev", def_p_diff_dev);
    const char* p_in_video = args_find_char(argc, argv, "--in-video", def_p_in_video);
    const char* p_out_frames = args_find_char(argc, argv, "--out-frames", def_p_out_frames);
    const char* p_out_bb = args_find_char(argc, argv, "--out-bb", def_p_out_bb);
    const char* p_out_stats = args_find_char(argc, argv, "--out-stats", def_p_out_stats);
    const int p_track_all = args_find(argc, argv, "--track-all");
    const int p_task_stats = args_find(argc, argv, "--task-stats");

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
    printf("#  * fra-start      = %d\n", p_fra_start);
    printf("#  * fra-end        = %d\n", p_fra_end);
    printf("#  * fra-skip       = %d\n", p_fra_skip);
    printf("#  * light-min      = %d\n", p_light_min);
    printf("#  * light-max      = %d\n", p_light_max);
    printf("#  * surface-min    = %d\n", p_surface_min);
    printf("#  * surface-max    = %d\n", p_surface_max);
    printf("#  * k              = %d\n", p_k);
    printf("#  * r-extrapol     = %d\n", p_r_extrapol);
    printf("#  * angle-max      = %f\n", p_angle_max);
    printf("#  * fra-star-min   = %d\n", p_fra_star_min);
    printf("#  * fra-meteor-min = %d\n", p_fra_meteor_min);
    printf("#  * fra-meteor-max = %d\n", p_fra_meteor_max);
    printf("#  * diff-dev       = %4.2f\n", p_diff_dev);
    printf("#  * track-all      = %d\n", p_track_all);
    printf("#  * task-stats     = %d\n", p_task_stats);
    printf("#\n");
    printf("#  * Runtime mode   = Sequence\n");
    printf("#\n");

    // arguments checking
    if (!p_in_video) {
        fprintf(stderr, "(EE) '--in-video' is missing\n");
        exit(1);
    }
    if (p_fra_star_min < 2) {
        fprintf(stderr, "(EE) '--fra-star-min' has to be bigger than 1\n");
        exit(1);
    }
    if (p_fra_meteor_min < 2) {
        fprintf(stderr, "(EE) '--fra-meteor-min' has to be bigger than 1\n");
        exit(1);
    }
    if (p_fra_meteor_max < p_fra_meteor_min) {
        fprintf(stderr, "(EE) '--fra-meteor-max' has to be bigger than '--fra-meteor-min'\n");
        exit(1);
    }
    if ((p_fra_end - p_fra_start) > MAX_N_FRAMES) {
        fprintf(stderr, "(EE) '--fra-end' - '--fra-start' has to be lower than %d\n", MAX_N_FRAMES);
        exit(1);
    }
    if (p_fra_end < p_fra_start) {
        fprintf(stderr, "(EE) '--fra-end' has to be higher than '--fra-start'\n");
        exit(1);
    }

    // -------------------------------- //
    // -- INITIALISATION GLOBAL DATA -- //
    // -------------------------------- //

    tracking_init_global_data();

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    // objects allocation
    const size_t b = 1; // image border
    const size_t n_ffmpeg_threads = 4; // 0 = use all the threads available
    Video2 video(p_in_video, p_fra_start, p_fra_end, p_fra_skip, n_ffmpeg_threads, b);
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
    Features_extractor extractor0(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    extractor0.set_custom_name("Extractor0");
    Features_merger merger0(i0, i1, j0, j1, b, p_surface_min, p_surface_max, MAX_ROI_SIZE);
    merger0.set_custom_name("Merger0");

    Threshold threshold_min1(i0, i1, j0, j1, b, p_light_min);
    threshold_min1.set_custom_name("Thr1<min>");
    Threshold threshold_max1(i0, i1, j0, j1, b, p_light_max);
    threshold_max1.set_custom_name("Thr1<max>");
    CCL_LSL lsl1(i0, i1, j0, j1, b);
    lsl1.set_custom_name("CCL_LSL1");
    Features_extractor extractor1(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    extractor1.set_custom_name("Extractor1");
    Features_merger merger1(i0, i1, j0, j1, b, p_surface_min, p_surface_max, MAX_ROI_SIZE);
    merger1.set_custom_name("Merger1");

    KNN_matcher matcher(i0, i1, j0, j1, p_k, MAX_ROI_SIZE);
    Features_motion motion(MAX_ROI_SIZE);
    motion.set_custom_name("Motion");
    Tracking tracking(p_r_extrapol, p_angle_max, p_diff_dev, p_track_all, p_fra_star_min, p_fra_meteor_min,
                      p_fra_meteor_max, MAX_ROI_SIZE, MAX_TRACKS_SIZE, MAX_N_FRAMES);
    Logger_ROI log_ROI(p_out_stats ? p_out_stats : "", MAX_ROI_SIZE, MAX_TRACKS_SIZE);
    Logger_KNN log_KNN(p_out_stats ? p_out_stats : "", i0, i1, j0, j1, MAX_ROI_SIZE);
    Logger_motion log_motion(p_out_stats ? p_out_stats : "");
    Logger_track log_track(p_out_stats ? p_out_stats : "", MAX_TRACKS_SIZE);
    Logger_frame log_frame(p_out_frames ? p_out_frames : "", i0, i1, j0, j1, b);
    log_motion.set_custom_name("Logger_motio");

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
    extractor0[ftr_ext::sck::extract::in_img] = lsl0[ccl::sck::apply::out_img];
    extractor0[ftr_ext::sck::extract::in_n_ROI] = lsl0[ccl::sck::apply::out_n_ROI];

    lsl1[ccl::sck::apply::in_img] = threshold_min1[thr::sck::apply::out_img];
    extractor1[ftr_ext::sck::extract::in_img] = lsl1[ccl::sck::apply::out_img];
    extractor1[ftr_ext::sck::extract::in_n_ROI] = lsl1[ccl::sck::apply::out_n_ROI];

    // Step 3 : seuillage hysteresis && filter surface
    merger0[ftr_mrg::sck::merge::in_img1] = lsl0[ccl::sck::apply::out_img];
    merger0[ftr_mrg::sck::merge::in_img2] = threshold_max0[thr::sck::apply::out_img];
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

    merger1[ftr_mrg::sck::merge::in_img1] = lsl1[ccl::sck::apply::out_img];
    merger1[ftr_mrg::sck::merge::in_img2] = threshold_max1[thr::sck::apply::out_img];
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

    // Step 4 : mise en correspondance
    matcher[knn::sck::match::in_ROI0_id] = merger0[ftr_mrg::sck::merge::out_ROI_id];
    matcher[knn::sck::match::in_ROI0_x] = merger0[ftr_mrg::sck::merge::out_ROI_x];
    matcher[knn::sck::match::in_ROI0_y] = merger0[ftr_mrg::sck::merge::out_ROI_y];
    matcher[knn::sck::match::in_n_ROI0] = merger0[ftr_mrg::sck::merge::out_n_ROI];
    matcher[knn::sck::match::in_ROI1_id] = merger1[ftr_mrg::sck::merge::out_ROI_id];
    matcher[knn::sck::match::in_ROI1_x] = merger1[ftr_mrg::sck::merge::out_ROI_x];
    matcher[knn::sck::match::in_ROI1_y] = merger1[ftr_mrg::sck::merge::out_ROI_y];
    matcher[knn::sck::match::in_n_ROI1] = merger1[ftr_mrg::sck::merge::out_n_ROI];

    // Step 5 : recalage
    motion[ftr_mtn::sck::compute::in_ROI0_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
    motion[ftr_mtn::sck::compute::in_ROI0_x] = merger0[ftr_mrg::sck::merge::out_ROI_x];
    motion[ftr_mtn::sck::compute::in_ROI0_y] = merger0[ftr_mrg::sck::merge::out_ROI_y];
    motion[ftr_mtn::sck::compute::in_n_ROI0] = merger0[ftr_mrg::sck::merge::out_n_ROI];
    motion[ftr_mtn::sck::compute::in_ROI1_x] = merger1[ftr_mrg::sck::merge::out_ROI_x];
    motion[ftr_mtn::sck::compute::in_ROI1_y] = merger1[ftr_mrg::sck::merge::out_ROI_y];

    // Step 6 : tracking
    tracking[trk::sck::perform::in_frame] = video[vid2::sck::generate::out_frame];
    tracking[trk::sck::perform::in_ROI0_id] = merger0[ftr_mrg::sck::merge::out_ROI_id];
    tracking[trk::sck::perform::in_ROI0_xmin] = merger0[ftr_mrg::sck::merge::out_ROI_xmin];
    tracking[trk::sck::perform::in_ROI0_xmax] = merger0[ftr_mrg::sck::merge::out_ROI_xmax];
    tracking[trk::sck::perform::in_ROI0_ymin] = merger0[ftr_mrg::sck::merge::out_ROI_ymin];
    tracking[trk::sck::perform::in_ROI0_ymax] = merger0[ftr_mrg::sck::merge::out_ROI_ymax];
    tracking[trk::sck::perform::in_ROI0_x] = merger0[ftr_mrg::sck::merge::out_ROI_x];
    tracking[trk::sck::perform::in_ROI0_y] = merger0[ftr_mrg::sck::merge::out_ROI_y];
    tracking[trk::sck::perform::in_ROI0_error] = motion[ftr_mtn::sck::compute::out_ROI0_error];
    tracking[trk::sck::perform::in_ROI0_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
    tracking[trk::sck::perform::in_n_ROI0] = merger0[ftr_mrg::sck::merge::out_n_ROI];
    tracking[trk::sck::perform::in_ROI1_id] = merger1[ftr_mrg::sck::merge::out_ROI_id];
    tracking[trk::sck::perform::in_ROI1_xmin] = merger1[ftr_mrg::sck::merge::out_ROI_xmin];
    tracking[trk::sck::perform::in_ROI1_xmax] = merger1[ftr_mrg::sck::merge::out_ROI_xmax];
    tracking[trk::sck::perform::in_ROI1_ymin] = merger1[ftr_mrg::sck::merge::out_ROI_ymin];
    tracking[trk::sck::perform::in_ROI1_ymax] = merger1[ftr_mrg::sck::merge::out_ROI_ymax];
    tracking[trk::sck::perform::in_ROI1_x] = merger1[ftr_mrg::sck::merge::out_ROI_x];
    tracking[trk::sck::perform::in_ROI1_y] = merger1[ftr_mrg::sck::merge::out_ROI_y];
    tracking[trk::sck::perform::in_ROI1_prev_id] = matcher[knn::sck::match::out_ROI1_prev_id];
    tracking[trk::sck::perform::in_n_ROI1] = merger1[ftr_mrg::sck::merge::out_n_ROI];
    tracking[trk::sck::perform::in_theta] = motion[ftr_mtn::sck::compute::out_theta];
    tracking[trk::sck::perform::in_tx] = motion[ftr_mtn::sck::compute::out_tx];
    tracking[trk::sck::perform::in_ty] = motion[ftr_mtn::sck::compute::out_ty];
    tracking[trk::sck::perform::in_mean_error] = motion[ftr_mtn::sck::compute::out_mean_error];
    tracking[trk::sck::perform::in_std_deviation] = motion[ftr_mtn::sck::compute::out_std_deviation];

    if (p_out_frames) {
        log_frame[lgr_fra::sck::write::in_img] =  merger1[ftr_mrg::sck::merge::out_img];
        log_frame[lgr_fra::sck::write::in_frame] =   video[vid2::sck::generate::out_frame];
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
        log_ROI[lgr_roi::sck::write::in_n_ROI1] = merger1[ftr_mrg::sck::merge::out_n_ROI];
        log_ROI[lgr_roi::sck::write::in_track_id] = tracking[trk::sck::perform::out_track_id];
        log_ROI[lgr_roi::sck::write::in_track_end] = tracking[trk::sck::perform::out_track_end];
        log_ROI[lgr_roi::sck::write::in_track_obj_type] = tracking[trk::sck::perform::out_track_obj_type];
        log_ROI[lgr_roi::sck::write::in_n_tracks] = tracking[trk::sck::perform::out_n_tracks];
        log_ROI[lgr_roi::sck::write::in_frame] = video[vid2::sck::generate::out_frame];

        log_KNN[lgr_knn::sck::write::in_data_nearest] = matcher[knn::sck::match::out_data_nearest];
        log_KNN[lgr_knn::sck::write::in_data_distances] = matcher[knn::sck::match::out_data_distances];
        log_KNN[lgr_knn::sck::write::in_ROI_id] = merger0[ftr_mrg::sck::merge::out_ROI_id]; // pas sur
        log_KNN[lgr_knn::sck::write::in_ROI_S] = merger0[ftr_mrg::sck::merge::out_ROI_S]; // pas sur
        log_KNN[lgr_knn::sck::write::in_ROI_dx] = motion[ftr_mtn::sck::compute::out_ROI0_dx];
        log_KNN[lgr_knn::sck::write::in_ROI_dy] = motion[ftr_mtn::sck::compute::out_ROI0_dy];
        log_KNN[lgr_knn::sck::write::in_ROI_error] = motion[ftr_mtn::sck::compute::out_ROI0_error];
        log_KNN[lgr_knn::sck::write::in_ROI_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
        log_KNN[lgr_knn::sck::write::in_n_ROI] = merger0[ftr_mrg::sck::merge::out_n_ROI]; // pas sur
        log_KNN[lgr_knn::sck::write::in_frame] = video[vid2::sck::generate::out_frame];

        log_motion[lgr_mtn::sck::write::in_first_theta] = motion[ftr_mtn::sck::compute::out_first_theta];
        log_motion[lgr_mtn::sck::write::in_first_tx] = motion[ftr_mtn::sck::compute::out_first_tx];
        log_motion[lgr_mtn::sck::write::in_first_ty] = motion[ftr_mtn::sck::compute::out_first_ty];
        log_motion[lgr_mtn::sck::write::in_first_mean_error] = motion[ftr_mtn::sck::compute::out_first_mean_error];
        log_motion[lgr_mtn::sck::write::in_first_std_deviation] = motion[ftr_mtn::sck::compute::out_first_std_deviation];
        log_motion[lgr_mtn::sck::write::in_theta] = motion[ftr_mtn::sck::compute::out_theta];
        log_motion[lgr_mtn::sck::write::in_tx] = motion[ftr_mtn::sck::compute::out_tx];
        log_motion[lgr_mtn::sck::write::in_ty] = motion[ftr_mtn::sck::compute::out_ty];
        log_motion[lgr_mtn::sck::write::in_mean_error] = motion[ftr_mtn::sck::compute::out_mean_error];
        log_motion[lgr_mtn::sck::write::in_std_deviation] = motion[ftr_mtn::sck::compute::out_std_deviation];
        log_motion[lgr_mtn::sck::write::in_frame] = video[vid2::sck::generate::out_frame];

        log_track[lgr_trk::sck::write::in_track_id] = tracking[trk::sck::perform::out_track_id];
        log_track[lgr_trk::sck::write::in_track_begin] = tracking[trk::sck::perform::out_track_begin];
        log_track[lgr_trk::sck::write::in_track_end] = tracking[trk::sck::perform::out_track_end];
        log_track[lgr_trk::sck::write::in_track_obj_type] = tracking[trk::sck::perform::out_track_obj_type];
        log_track[lgr_trk::sck::write::in_n_tracks] = tracking[trk::sck::perform::out_n_tracks];
        log_track[lgr_trk::sck::write::in_frame] = video[vid2::sck::generate::out_frame];
    }

    // --------------------- //
    // -- CREATE SEQUENCE -- //
    // --------------------- //

    aff3ct::runtime::Sequence sequence_or_pipeline(video[vid2::tsk::generate], 1);

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
            size_t n_tracks = tracking_count_objects(tracking.get_track_array(), &n_stars, &n_meteors, &n_noise);
            fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\r", n_meteors,
                    n_stars, n_noise, n_tracks);
            fflush(stderr);
            n_frames++;
            return false;
        };

    printf("# The program is running...\n");

    sequence_or_pipeline.exec(stop_condition);

    // ------------------- //
    // -- PRINT RESULTS -- //
    // ------------------- //

    fprintf(stderr, "\n");
    if (p_out_bb)
        tracking_save_array_BB(p_out_bb, tracking.get_BB_array(), tracking.get_track_array(), MAX_N_FRAMES,
                               p_track_all);
    tracking_track_array_write(stdout, tracking.get_track_array());

    unsigned n_stars = 0, n_meteors = 0, n_noise = 0;
    size_t real_n_tracks = tracking_count_objects(tracking.get_track_array(), &n_stars, &n_meteors, &n_noise);
    printf("# Tracks statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames -1);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, real_n_tracks);

    // display the statistics of the tasks (if enabled)
    std::cout << "#" << std::endl;
    aff3ct::tools::Stats::show(sequence_or_pipeline.get_tasks_per_types(), true, false);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
