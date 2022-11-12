#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <nrc2.h>
#include <algorithm>
#include <memory>

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
#include "fmdt/Video/Video.hpp"
#include "fmdt/Images/Images.hpp"
#include "fmdt/Logger/Logger_ROI.hpp"
#include "fmdt/Logger/Logger_KNN.hpp"
#include "fmdt/Logger/Logger_motion.hpp"
#include "fmdt/Logger/Logger_track.hpp"
#include "fmdt/Logger/Logger_frame.hpp"

// Do not use this define anymore!! NOW it is set in the CMakeFile :-)
// #define FMDT_ENABLE_PIPELINE

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
    char* def_p_out_probes = NULL;
    int def_p_video_loop = 1;

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
                "  --out-probes        Path of the output probe vales, only required for benchmarking purpose [%s]\n",
                def_p_out_probes ? def_p_out_probes : "NULL");
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
                "  --video-buff        Bufferize all the video in global memory before executing the chain        \n");
        fprintf(stderr,
                "  --video-loop        Number of times the video is read in loop                              [%d]\n",
                def_p_video_loop);
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
    const char* p_out_probes = args_find_char(argc, argv, "--out-probes", def_p_out_probes);
    const int p_track_all = args_find(argc, argv, "--track-all");
    const int p_task_stats = args_find(argc, argv, "--task-stats");
    const int p_video_buff = args_find(argc, argv, "--video-buff");
    const int p_video_loop = args_find_int(argc, argv, "--video-loop", def_p_video_loop);

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
    printf("#  * out-probes     = %s\n", p_out_probes);
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
    printf("#  * video-buff     = %d\n", p_video_buff);
    printf("#  * video-loop     = %d\n", p_video_loop);
    printf("#\n");
#ifdef FMDT_ENABLE_PIPELINE
    printf("#  * Runtime mode   = Pipeline\n");
#else
    printf("#  * Runtime mode   = Sequence\n");
#endif
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
    if (!tools_is_dir(p_in_video) && p_video_buff)
        fprintf(stderr, "(WW) '--video-buff' has not effect when '--in-video' is a video file.\n");
    if (!tools_is_dir(p_in_video) && p_video_loop > 1)
        fprintf(stderr, "(WW) '--video-loop' has not effect when '--in-video' is a video file.\n");
    if (p_video_loop <= 0) {
        fprintf(stderr, "(EE) '--video-loop' has to be bigger than 0\n");
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
    std::unique_ptr<Video> video;
    std::unique_ptr<Images> images;
    size_t i0, i1, j0, j1;
    if (!tools_is_dir(p_in_video)) {
        video.reset(new Video(p_in_video, p_fra_start, p_fra_end, p_fra_skip, n_ffmpeg_threads, b));
        i0 = video->get_i0();
        i1 = video->get_i1();
        j0 = video->get_j0();
        j1 = video->get_j1();
    } else {
        images.reset(new Images(p_in_video, p_fra_start, p_fra_end, p_fra_skip, b, p_video_buff));
        i0 = images->get_i0();
        i1 = images->get_i1();
        j0 = images->get_j0();
        j1 = images->get_j1();
        images->set_loop_size(p_video_loop);
    }

    Threshold threshold_min(i0, i1, j0, j1, b, p_light_min);
    Threshold threshold_max(i0, i1, j0, j1, b, p_light_max);
    threshold_min.set_custom_name("Thr<min>");
    threshold_max.set_custom_name("Thr<max>");
    CCL_LSL lsl(i0, i1, j0, j1, b);
    Features_extractor extractor(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    extractor.set_custom_name("Extractor");
    Features_merger merger(i0, i1, j0, j1, b, p_surface_min, p_surface_max, MAX_ROI_SIZE);
    merger.set_custom_name("Merger");
    KNN_matcher matcher(i0, i1, j0, j1, p_k, MAX_ROI_SIZE);
    Features_motion motion(MAX_ROI_SIZE);
    motion.set_custom_name("Motion");
    Tracking tracking(p_r_extrapol, p_angle_max, p_diff_dev, p_track_all, p_fra_star_min, p_fra_meteor_min,
                      p_fra_meteor_max, p_out_bb, MAX_ROI_SIZE, MAX_TRACKS_SIZE, MAX_BB_LIST_SIZE);
    aff3ct::module::Delayer<uint16_t> delayer_ROI_id(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint16_t> delayer_ROI_xmin(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint16_t> delayer_ROI_xmax(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint16_t> delayer_ROI_ymin(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint16_t> delayer_ROI_ymax(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_S(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_Sx(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_Sy(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<float> delayer_ROI_x(MAX_ROI_SIZE, 0.f);
    aff3ct::module::Delayer<float> delayer_ROI_y(MAX_ROI_SIZE, 0.f);
    aff3ct::module::Delayer<uint32_t> delayer_n_ROI(1, 0);
    delayer_ROI_id.set_custom_name("D<ROI_id>");
    delayer_ROI_xmin.set_custom_name("D<ROI_xmin>");
    delayer_ROI_xmax.set_custom_name("D<ROI_xmax>");
    delayer_ROI_ymin.set_custom_name("D<ROI_ymin>");
    delayer_ROI_ymax.set_custom_name("D<ROI_ymax>");
    delayer_ROI_S.set_custom_name("D<ROI_S>");
    delayer_ROI_Sx.set_custom_name("D<ROI_Sx>");
    delayer_ROI_Sy.set_custom_name("D<ROI_Sy>");
    delayer_ROI_x.set_custom_name("D<ROI_x>");
    delayer_ROI_y.set_custom_name("D<ROI_y>");
    delayer_n_ROI.set_custom_name("D<n_ROI>");
    Logger_ROI log_ROI(p_out_stats ? p_out_stats : "", MAX_ROI_SIZE, MAX_TRACKS_SIZE);
    Logger_KNN log_KNN(p_out_stats ? p_out_stats : "", i0, i1, j0, j1, MAX_ROI_SIZE);
    Logger_motion log_motion(p_out_stats ? p_out_stats : "");
    Logger_track log_track(p_out_stats ? p_out_stats : "", MAX_TRACKS_SIZE);
    Logger_frame log_frame(p_out_frames ? p_out_frames : "", i0, i1, j0, j1, b);
    log_motion.set_custom_name("Logger_motio");

    // ------------------- //
    // -- TASKS BINDING -- //
    // ------------------- //

    // Step 0 : delais => caractéristiques des ROIs à t - 1
    delayer_ROI_id[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_ROI_xmin[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_ROI_xmax[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_ROI_ymin[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_ROI_ymax[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_ROI_S[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_ROI_Sx[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_ROI_Sy[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_ROI_x[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_ROI_y[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    delayer_n_ROI[aff3ct::module::dly::tsk::produce] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];

    // Step 1 : seuillage low/high
    threshold_min[thr::sck::apply::in_img] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    threshold_max[thr::sck::apply::in_img] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];

    // Step 2 : ECC/ACC
    lsl[ccl::sck::apply::in_img] = threshold_min[thr::sck::apply::out_img];

    extractor[ftr_ext::sck::extract::in_img] = lsl[ccl::sck::apply::out_img];
    extractor[ftr_ext::sck::extract::in_n_ROI] = lsl[ccl::sck::apply::out_n_ROI];

    // Step 3 : seuillage hysteresis && filter surface
    merger[ftr_mrg::sck::merge::in_img1] = lsl[ccl::sck::apply::out_img];
    merger[ftr_mrg::sck::merge::in_img2] = threshold_max[thr::sck::apply::out_img];
    merger[ftr_mrg::sck::merge::in_ROI_id] = extractor[ftr_ext::sck::extract::out_ROI_id];
    merger[ftr_mrg::sck::merge::in_ROI_xmin] = extractor[ftr_ext::sck::extract::out_ROI_xmin];
    merger[ftr_mrg::sck::merge::in_ROI_xmax] = extractor[ftr_ext::sck::extract::out_ROI_xmax];
    merger[ftr_mrg::sck::merge::in_ROI_ymin] = extractor[ftr_ext::sck::extract::out_ROI_ymin];
    merger[ftr_mrg::sck::merge::in_ROI_ymax] = extractor[ftr_ext::sck::extract::out_ROI_ymax];
    merger[ftr_mrg::sck::merge::in_ROI_S] = extractor[ftr_ext::sck::extract::out_ROI_S];
    merger[ftr_mrg::sck::merge::in_ROI_Sx] = extractor[ftr_ext::sck::extract::out_ROI_Sx];
    merger[ftr_mrg::sck::merge::in_ROI_Sy] = extractor[ftr_ext::sck::extract::out_ROI_Sy];
    merger[ftr_mrg::sck::merge::in_ROI_x] = extractor[ftr_ext::sck::extract::out_ROI_x];
    merger[ftr_mrg::sck::merge::in_ROI_y] = extractor[ftr_ext::sck::extract::out_ROI_y];
    merger[ftr_mrg::sck::merge::in_n_ROI] = lsl[ccl::sck::apply::out_n_ROI];

    // Step 4 : mise en correspondance
    matcher[knn::sck::match::in_ROI0_id] = delayer_ROI_id[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_ROI0_x] = delayer_ROI_x[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_ROI0_y] = delayer_ROI_y[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_n_ROI0] = delayer_n_ROI[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_ROI1_id] = merger[ftr_mrg::sck::merge::out_ROI_id];
    matcher[knn::sck::match::in_ROI1_x] = merger[ftr_mrg::sck::merge::out_ROI_x];
    matcher[knn::sck::match::in_ROI1_y] = merger[ftr_mrg::sck::merge::out_ROI_y];
    matcher[knn::sck::match::in_n_ROI1] = merger[ftr_mrg::sck::merge::out_n_ROI];

    // Step 5 : recalage
    motion[ftr_mtn::sck::compute::in_ROI0_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
    motion[ftr_mtn::sck::compute::in_ROI0_x] = delayer_ROI_x[aff3ct::module::dly::sck::produce::out];
    motion[ftr_mtn::sck::compute::in_ROI0_y] = delayer_ROI_y[aff3ct::module::dly::sck::produce::out];
    motion[ftr_mtn::sck::compute::in_n_ROI0] = delayer_n_ROI[aff3ct::module::dly::sck::produce::out];
    motion[ftr_mtn::sck::compute::in_ROI1_x] = merger[ftr_mrg::sck::merge::out_ROI_x];
    motion[ftr_mtn::sck::compute::in_ROI1_y] = merger[ftr_mrg::sck::merge::out_ROI_y];

    // Step 6 : tracking
    tracking[trk::sck::perform::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];
    tracking[trk::sck::perform::in_ROI0_id] = delayer_ROI_id[aff3ct::module::dly::sck::produce::out];
    tracking[trk::sck::perform::in_ROI0_xmin] = delayer_ROI_xmin[aff3ct::module::dly::sck::produce::out];
    tracking[trk::sck::perform::in_ROI0_xmax] = delayer_ROI_xmax[aff3ct::module::dly::sck::produce::out];
    tracking[trk::sck::perform::in_ROI0_ymin] = delayer_ROI_ymin[aff3ct::module::dly::sck::produce::out];
    tracking[trk::sck::perform::in_ROI0_ymax] = delayer_ROI_ymax[aff3ct::module::dly::sck::produce::out];
    tracking[trk::sck::perform::in_ROI0_x] = delayer_ROI_x[aff3ct::module::dly::sck::produce::out];
    tracking[trk::sck::perform::in_ROI0_y] = delayer_ROI_y[aff3ct::module::dly::sck::produce::out];
    tracking[trk::sck::perform::in_ROI0_error] = motion[ftr_mtn::sck::compute::out_ROI0_error];
    tracking[trk::sck::perform::in_ROI0_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
    tracking[trk::sck::perform::in_n_ROI0] = delayer_n_ROI[aff3ct::module::dly::sck::produce::out];
    tracking[trk::sck::perform::in_ROI1_id] = merger[ftr_mrg::sck::merge::out_ROI_id];
    tracking[trk::sck::perform::in_ROI1_xmin] = merger[ftr_mrg::sck::merge::out_ROI_xmin];
    tracking[trk::sck::perform::in_ROI1_xmax] = merger[ftr_mrg::sck::merge::out_ROI_xmax];
    tracking[trk::sck::perform::in_ROI1_ymin] = merger[ftr_mrg::sck::merge::out_ROI_ymin];
    tracking[trk::sck::perform::in_ROI1_ymax] = merger[ftr_mrg::sck::merge::out_ROI_ymax];
    tracking[trk::sck::perform::in_ROI1_x] = merger[ftr_mrg::sck::merge::out_ROI_x];
    tracking[trk::sck::perform::in_ROI1_y] = merger[ftr_mrg::sck::merge::out_ROI_y];
    tracking[trk::sck::perform::in_ROI1_prev_id] = matcher[knn::sck::match::out_ROI1_prev_id];
    tracking[trk::sck::perform::in_n_ROI1] = merger[ftr_mrg::sck::merge::out_n_ROI];
    tracking[trk::sck::perform::in_theta] = motion[ftr_mtn::sck::compute::out_theta];
    tracking[trk::sck::perform::in_tx] = motion[ftr_mtn::sck::compute::out_tx];
    tracking[trk::sck::perform::in_ty] = motion[ftr_mtn::sck::compute::out_ty];
    tracking[trk::sck::perform::in_mean_error] = motion[ftr_mtn::sck::compute::out_mean_error];
    tracking[trk::sck::perform::in_std_deviation] = motion[ftr_mtn::sck::compute::out_std_deviation];

    delayer_ROI_id[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_xmin[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_xmin];
    delayer_ROI_xmax[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_xmax];
    delayer_ROI_ymin[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_ymin];
    delayer_ROI_ymax[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_ymax];
    delayer_ROI_S[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_S];
    delayer_ROI_Sx[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_Sx];
    delayer_ROI_Sy[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_Sy];
    delayer_ROI_x[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_x];
    delayer_ROI_y[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_y];
    delayer_n_ROI[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_n_ROI];

    if (p_out_stats) {
        log_ROI[lgr_roi::sck::write::in_ROI0_id] = delayer_ROI_id[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI0_xmin] = delayer_ROI_xmin[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI0_xmax] = delayer_ROI_xmax[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI0_ymin] = delayer_ROI_ymin[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI0_ymax] = delayer_ROI_ymax[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI0_S] = delayer_ROI_S[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI0_Sx] = delayer_ROI_Sx[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI0_Sy] = delayer_ROI_Sy[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI0_x] = delayer_ROI_x[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI0_y] = delayer_ROI_y[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_n_ROI0] = delayer_n_ROI[aff3ct::module::dly::sck::produce::out];
        log_ROI[lgr_roi::sck::write::in_ROI1_id] = merger[ftr_mrg::sck::merge::out_ROI_id];
        log_ROI[lgr_roi::sck::write::in_ROI1_xmin] = merger[ftr_mrg::sck::merge::out_ROI_xmin];
        log_ROI[lgr_roi::sck::write::in_ROI1_xmax] = merger[ftr_mrg::sck::merge::out_ROI_xmax];
        log_ROI[lgr_roi::sck::write::in_ROI1_ymin] = merger[ftr_mrg::sck::merge::out_ROI_ymin];
        log_ROI[lgr_roi::sck::write::in_ROI1_ymax] = merger[ftr_mrg::sck::merge::out_ROI_ymax];
        log_ROI[lgr_roi::sck::write::in_ROI1_S] = merger[ftr_mrg::sck::merge::out_ROI_S];
        log_ROI[lgr_roi::sck::write::in_ROI1_Sx] = merger[ftr_mrg::sck::merge::out_ROI_Sx];
        log_ROI[lgr_roi::sck::write::in_ROI1_Sy] = merger[ftr_mrg::sck::merge::out_ROI_Sy];
        log_ROI[lgr_roi::sck::write::in_ROI1_x] = merger[ftr_mrg::sck::merge::out_ROI_x];
        log_ROI[lgr_roi::sck::write::in_ROI1_y] = merger[ftr_mrg::sck::merge::out_ROI_y];
        log_ROI[lgr_roi::sck::write::in_n_ROI1] = merger[ftr_mrg::sck::merge::out_n_ROI];
        log_ROI[lgr_roi::sck::write::in_track_id] = tracking[trk::sck::perform::out_track_id];
        log_ROI[lgr_roi::sck::write::in_track_end] = tracking[trk::sck::perform::out_track_end];
        log_ROI[lgr_roi::sck::write::in_track_obj_type] = tracking[trk::sck::perform::out_track_obj_type];
        log_ROI[lgr_roi::sck::write::in_n_tracks] = tracking[trk::sck::perform::out_n_tracks];
        log_ROI[lgr_roi::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];

        log_KNN[lgr_knn::sck::write::in_data_nearest] = matcher[knn::sck::match::out_data_nearest];
        log_KNN[lgr_knn::sck::write::in_data_distances] = matcher[knn::sck::match::out_data_distances];
        log_KNN[lgr_knn::sck::write::in_ROI_id] = delayer_ROI_id[aff3ct::module::dly::sck::produce::out];
        log_KNN[lgr_knn::sck::write::in_ROI_S] = delayer_ROI_S[aff3ct::module::dly::sck::produce::out];
        log_KNN[lgr_knn::sck::write::in_ROI_dx] = motion[ftr_mtn::sck::compute::out_ROI0_dx];
        log_KNN[lgr_knn::sck::write::in_ROI_dy] = motion[ftr_mtn::sck::compute::out_ROI0_dy];
        log_KNN[lgr_knn::sck::write::in_ROI_error] = motion[ftr_mtn::sck::compute::out_ROI0_error];
        log_KNN[lgr_knn::sck::write::in_ROI_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
        log_KNN[lgr_knn::sck::write::in_n_ROI] = delayer_n_ROI[aff3ct::module::dly::sck::produce::out];
        log_KNN[lgr_knn::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];

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
        log_motion[lgr_mtn::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];

        log_track[lgr_trk::sck::write::in_track_id] = tracking[trk::sck::perform::out_track_id];
        log_track[lgr_trk::sck::write::in_track_begin] = tracking[trk::sck::perform::out_track_begin];
        log_track[lgr_trk::sck::write::in_track_end] = tracking[trk::sck::perform::out_track_end];
        log_track[lgr_trk::sck::write::in_track_obj_type] = tracking[trk::sck::perform::out_track_obj_type];
        log_track[lgr_trk::sck::write::in_n_tracks] = tracking[trk::sck::perform::out_n_tracks];
        log_track[lgr_trk::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];
    }

    if (p_out_frames) {
        log_frame[lgr_fra::sck::write::in_img] = merger[ftr_mrg::sck::merge::out_img];
        log_frame[lgr_fra::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];
    }

    // create reporters and probes for the real-time probes file
    size_t inter_frame_lvl = 1;
    aff3ct::tools::Reporter_probe rep_fra_stats("Frame Counter", inter_frame_lvl);
    std::unique_ptr<aff3ct::module::Probe<>> prb_fra_id(rep_fra_stats.create_probe_occurrence("ID"));

    aff3ct::tools::Reporter_probe rep_thr_stats("Throughput, latency", "and time", inter_frame_lvl);
    std::unique_ptr<aff3ct::module::Probe<>> prb_thr_thr  (rep_thr_stats.create_probe_throughput("FPS"));
    std::unique_ptr<aff3ct::module::Probe<>> prb_thr_lat  (rep_thr_stats.create_probe_latency   ("LAT")); // only valid for sequence, invalid for pipeline
    std::unique_ptr<aff3ct::module::Probe<>> prb_thr_time (rep_thr_stats.create_probe_time      ("TIME"));
    const uint64_t mod = 1000000ul * 60ul * 10; // limit to 10 minutes timestamp
    std::unique_ptr<aff3ct::module::Probe<>> prb_thr_tstab(rep_thr_stats.create_probe_timestamp ("TSTAB", mod)); // timestamp begin
    std::unique_ptr<aff3ct::module::Probe<>> prb_thr_tstae(rep_thr_stats.create_probe_timestamp ("TSTAE", mod)); // timestamp end

    const std::vector<aff3ct::tools::Reporter*>& reporters = { &rep_fra_stats, &rep_thr_stats };
    aff3ct::tools::Terminal_dump terminal_probes(reporters);

    std::ofstream rt_probes_file;
    if (p_out_probes) {
        rt_probes_file.open(p_out_probes);
        rt_probes_file << "####################" << std::endl;
        rt_probes_file << "# Real-time probes #" << std::endl;
        rt_probes_file << "####################" << std::endl;
        terminal_probes.legend(rt_probes_file);

        // bind probes
        if (video)
            (*video)[vid::tsk::generate] = (*prb_thr_tstab)[aff3ct::module::prb::sck::probe_noin::status];
        else
            (*images)[img::tsk::generate] = (*prb_thr_tstab)[aff3ct::module::prb::sck::probe_noin::status];
        (*prb_fra_id   )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_thr  )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_lat  )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_time )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_tstae)[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
    }

    // determine the first task in the tasks graph depending on the command line parameters
    aff3ct::runtime::Task* first_task = nullptr;
    if (p_out_probes)
        first_task = &(*prb_thr_tstab)[aff3ct::module::prb::tsk::probe];
    else
        first_task = video ? &(*video)[vid::tsk::generate] : &(*images)[img::tsk::generate];

    // --------------------- //
    // -- CREATE SEQUENCE -- //
    // --------------------- //

#ifdef FMDT_ENABLE_PIPELINE
    // pipeline definition with separation stages
    std::vector<std::tuple<std::vector<aff3ct::runtime::Task*>,
                           std::vector<aff3ct::runtime::Task*>,
                           std::vector<aff3ct::runtime::Task*>>> sep_stages =
    { // pipeline stage 0
      std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                      std::vector<aff3ct::runtime::Task*>>(
        { first_task, },
        { video ? &(*video)[vid::tsk::generate] : &(*images)[img::tsk::generate], },
        { /* no exclusions in this stage */ } ),
      // pipeline stage 1
      std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                      std::vector<aff3ct::runtime::Task*>>(
        { &threshold_min[thr::tsk::apply], &threshold_max[thr::tsk::apply] },
        { &merger[ftr_mrg::tsk::merge], },
        { } ),
      // pipeline stage 2
      std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                      std::vector<aff3ct::runtime::Task*>>(
        { &delayer_ROI_id[aff3ct::module::dly::tsk::produce],
          &delayer_ROI_xmin[aff3ct::module::dly::tsk::produce],
          &delayer_ROI_xmax[aff3ct::module::dly::tsk::produce],
          &delayer_ROI_ymin[aff3ct::module::dly::tsk::produce],
          &delayer_ROI_ymax[aff3ct::module::dly::tsk::produce],
          &delayer_ROI_S[aff3ct::module::dly::tsk::produce],
          &delayer_ROI_Sx[aff3ct::module::dly::tsk::produce],
          &delayer_ROI_Sy[aff3ct::module::dly::tsk::produce],
          &delayer_ROI_x[aff3ct::module::dly::tsk::produce],
          &delayer_ROI_y[aff3ct::module::dly::tsk::produce],
          &delayer_n_ROI[aff3ct::module::dly::tsk::produce],
          &matcher[knn::tsk::match],
          &motion[ftr_mtn::tsk::compute],
          &tracking[trk::tsk::perform],
          &delayer_ROI_id[aff3ct::module::dly::tsk::memorize],
          &delayer_ROI_xmin[aff3ct::module::dly::tsk::memorize],
          &delayer_ROI_xmax[aff3ct::module::dly::tsk::memorize],
          &delayer_ROI_ymin[aff3ct::module::dly::tsk::memorize],
          &delayer_ROI_ymax[aff3ct::module::dly::tsk::memorize],
          &delayer_ROI_S[aff3ct::module::dly::tsk::memorize],
          &delayer_ROI_Sx[aff3ct::module::dly::tsk::memorize],
          &delayer_ROI_Sy[aff3ct::module::dly::tsk::memorize],
          &delayer_ROI_x[aff3ct::module::dly::tsk::memorize],
          &delayer_ROI_y[aff3ct::module::dly::tsk::memorize],
          &delayer_n_ROI[aff3ct::module::dly::tsk::memorize],
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
        std::get<0>(sep_stages[2]).push_back(&log_frame[lgr_fra::tsk::write]);
    }

    aff3ct::runtime::Pipeline sequence_or_pipeline({ first_task }, // first task of the sequence
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
    aff3ct::runtime::Sequence sequence_or_pipeline(*first_task, 1);
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

    std::chrono::time_point<std::chrono::steady_clock> t_start;
    unsigned n_frames = 0;
    std::function<bool(const std::vector<const int*>&)> stop_condition =
        [&tracking, &n_frames, &terminal_probes, &rt_probes_file, &t_start] (const std::vector<const int*>& statuses) {
            if (statuses.back() != nullptr) {
                fprintf(stderr, "(II) Frame n°%4u", n_frames);
                unsigned n_stars = 0, n_meteors = 0, n_noise = 0;
                size_t n_tracks = tracking_count_objects(tracking.get_track_array(), &n_stars, &n_meteors, &n_noise);

                auto t_stop = std::chrono::steady_clock::now();
                auto time_duration = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t_stop - t_start).count();
                auto time_duration_sec = time_duration * 1e-6;

                fprintf(stderr, " -- Time = %6.3f sec", time_duration_sec);
                fprintf(stderr, " -- FPS = %4d", (int)(n_frames / time_duration_sec));
                fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\r", n_meteors,
                        n_stars, n_noise, (unsigned long)n_tracks);
                fflush(stderr);
                n_frames++;
                if (rt_probes_file.is_open())
                    terminal_probes.temp_report(rt_probes_file);
            }
            return false;
        };

    printf("# The program is running...\n");

    t_start = std::chrono::steady_clock::now();
#ifdef FMDT_ENABLE_PIPELINE
    sequence_or_pipeline.exec({
        [] (const std::vector<const int*>& statuses) { return false; }, // stop condition stage 0
        [] (const std::vector<const int*>& statuses) { return false; }, // stop condition stage 1
        stop_condition});                                               // stop condition stage 2
#else
    sequence_or_pipeline.exec(stop_condition);
#endif
    auto t_stop = std::chrono::steady_clock::now();
    auto time_duration = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t_stop - t_start).count();
    auto time_duration_sec = time_duration * 1e-6;

    // ------------------- //
    // -- PRINT RESULTS -- //
    // ------------------- //

    if (rt_probes_file.is_open())
        terminal_probes.final_report(rt_probes_file);

    fprintf(stderr, "\n");
    if (p_out_bb)
        tracking_save_array_BB(p_out_bb, tracking.get_BB_array(), tracking.get_track_array(), MAX_N_FRAMES,
                               p_track_all);
    tracking_track_array_write(stdout, tracking.get_track_array());

    unsigned n_stars = 0, n_meteors = 0, n_noise = 0;
    size_t real_n_tracks = tracking_count_objects(tracking.get_track_array(), &n_stars, &n_meteors, &n_noise);
    printf("# Tracks statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, (unsigned long)real_n_tracks);
    printf("# -> Took %f seconds (avg %d FPS)\n", time_duration_sec, (int)(n_frames / time_duration_sec));

    // display the statistics of the tasks (if enabled)
#ifdef FMDT_ENABLE_PIPELINE
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

#ifdef FMDT_ENABLE_PIPELINE
    sequence_or_pipeline.unbind_adaptors();
#endif

    return EXIT_SUCCESS;
}
