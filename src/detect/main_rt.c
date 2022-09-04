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
#include "fmdt/Delayer/Delayer.hpp"
#include "fmdt/Features_extractor/Features_extractor.hpp"
#include "fmdt/Features_merger/Features_merger.hpp"
#include "fmdt/Features_motion/Features_motion.hpp"
#include "fmdt/KNN_matcher/KNN_matcher.hpp"
#include "fmdt/Threshold/Threshold.hpp"
#include "fmdt/Tracking/Tracking.hpp"
#include "fmdt/Video/Video.hpp"

int main(int argc, char** argv) {
    // default values
    int def_p_fra_start = 0;
    int def_p_fra_end = MAX_N_FRAMES;
    int def_p_skip_fra = 0;
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
                "  --out-stats         TODO! Path to folder                                                   [%s]\n",
                def_p_out_stats ? def_p_out_stats : "NULL");
        fprintf(stderr,
                "  --fra-start         Starting point of the video                                            [%d]\n",
                def_p_fra_start);
        fprintf(stderr,
                "  --fra-end           Ending point of the video                                              [%d]\n",
                def_p_fra_end);
        fprintf(stderr,
                "  --skip-fra          Number of skipped frames                                               [%d]\n",
                def_p_skip_fra);
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
                "  -h                  This help                                                                  \n");
        exit(1);
    }

    // Parsing Arguments
    const int p_fra_start = args_find_int(argc, argv, "--fra-start", def_p_fra_start);
    const int p_fra_end = args_find_int(argc, argv, "--fra-end", def_p_fra_end);
    const int p_skip_fra = args_find_int(argc, argv, "--skip-fra", def_p_skip_fra);
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
    printf("#  * skip-fra       = %d\n", p_skip_fra);
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
    if (!p_out_frames)
        fprintf(stderr, "(II) '--out-frames' is missing -> no frames will be saved\n");
    if (!p_out_stats)
        fprintf(stderr, "(II) '--out-stats' is missing -> no stats will be saved\n");

    // -------------------------------- //
    // -- INITIALISATION GLOBAL DATA -- //
    // -------------------------------- //

    tracking_init_global_data();

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    // objects allocation
    int b = 1; // image border
    Video video(p_in_video, p_fra_start, p_fra_end, p_skip_fra, b);
    const int i0 = video.get_i0();
    const int i1 = video.get_i1();
    const int j0 = video.get_j0();
    const int j1 = video.get_j1();

    Threshold threshold_min(i0 - b, i1 + b, j0 - b, j1 + b, p_light_min);
    Threshold threshold_max(i0 - b, i1 + b, j0 - b, j1 + b, p_light_max);
    CCL_LSL lsl(i0, i1, j0, j1, b);
    Features_extractor extractor(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    Features_merger merger(i0, i1, j0, j1, b, p_surface_min, p_surface_max, MAX_ROI_SIZE);
    KNN_matcher matcher(i0, i1, j0, j1, p_k, MAX_ROI_SIZE);
    Features_motion motion(MAX_ROI_SIZE);
    Tracking tracking(p_r_extrapol, p_angle_max, p_diff_dev, p_track_all, p_fra_star_min, p_fra_meteor_min,
                      p_fra_meteor_max, MAX_ROI_SIZE, MAX_TRACKS_SIZE, MAX_N_FRAMES);
    Delayer<uint16_t> delayer_ROI_id(MAX_ROI_SIZE, 0);
    Delayer<uint16_t> delayer_ROI_xmin(MAX_ROI_SIZE, 0);
    Delayer<uint16_t> delayer_ROI_xmax(MAX_ROI_SIZE, 0);
    Delayer<uint16_t> delayer_ROI_ymin(MAX_ROI_SIZE, 0);
    Delayer<uint16_t> delayer_ROI_ymax(MAX_ROI_SIZE, 0);
    Delayer<uint32_t> delayer_ROI_S(MAX_ROI_SIZE, 0);
    Delayer<float> delayer_ROI_x(MAX_ROI_SIZE, 0.f);
    Delayer<float> delayer_ROI_y(MAX_ROI_SIZE, 0.f);
    Delayer<int32_t> delayer_ROI_prev_id(MAX_ROI_SIZE, 0);
    Delayer<uint32_t> delayer_ROI_frame(MAX_ROI_SIZE, 0);
    Delayer<int32_t> delayer_ROI_time(MAX_ROI_SIZE, 0);
    Delayer<int32_t> delayer_ROI_time_motion(MAX_ROI_SIZE, 0);
    Delayer<uint8_t> delayer_ROI_is_extrapolated(MAX_ROI_SIZE, 0);
    Delayer<uint32_t> delayer_n_ROI(1, 0);

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//

    printf("# The program is running...\n");
    size_t real_n_tracks;
    unsigned n_frames = 0, n_stars = 0, n_meteors = 0, n_noise = 0;
    video[vid::tsk::generate].exec();
    while (video[vid::tsk::generate].get_status()[0] == aff3ct::module::status_t::SUCCESS) {
        const uint32_t frame = *(const uint32_t*)video[vid::sck::generate::out_frame].get_dataptr();
        assert(frame < MAX_N_FRAMES);
        fprintf(stderr, "(II) Frame n°%4u", frame);

        delayer_ROI_id[dly::tsk::produce].exec();
        delayer_ROI_xmin[dly::tsk::produce].exec();
        delayer_ROI_xmax[dly::tsk::produce].exec();
        delayer_ROI_ymin[dly::tsk::produce].exec();
        delayer_ROI_ymax[dly::tsk::produce].exec();
        delayer_ROI_S[dly::tsk::produce].exec();
        delayer_ROI_x[dly::tsk::produce].exec();
        delayer_ROI_y[dly::tsk::produce].exec();
        delayer_ROI_prev_id[dly::tsk::produce].exec();
        delayer_ROI_frame[dly::tsk::produce].exec();
        delayer_ROI_time[dly::tsk::produce].exec();
        delayer_ROI_time_motion[dly::tsk::produce].exec();
        delayer_ROI_is_extrapolated[dly::tsk::produce].exec();
        delayer_n_ROI[dly::tsk::produce].exec();

        // Step 1 : seuillage low/high
        threshold_min[thr::sck::apply::in_img] = video[vid::sck::generate::out_img];
        threshold_max[thr::sck::apply::in_img] = video[vid::sck::generate::out_img];
        threshold_min[thr::tsk::apply].exec();
        threshold_max[thr::tsk::apply].exec();

        // Step 2 : ECC/ACC
        lsl[ccl::sck::apply::in_img] = threshold_min[thr::sck::apply::out_img];
        lsl[ccl::tsk::apply].exec();

        extractor[ftr_ext::sck::extract::in_img] = lsl[ccl::sck::apply::out_img];
        extractor[ftr_ext::sck::extract::in_n_ROI] = lsl[ccl::sck::apply::out_n_ROI];
        extractor[ftr_ext::tsk::extract].exec();

        // Step 3 : seuillage hysteresis && filter surface
        merger[ftr_mrg::sck::merge::in_img1] = lsl[ccl::sck::apply::out_img];
        merger[ftr_mrg::sck::merge::in_img2] = threshold_max[thr::sck::apply::out_img];
        merger[ftr_mrg::sck::merge::in_ROI_id] = extractor[ftr_ext::sck::extract::out_ROI_id];
        merger[ftr_mrg::sck::merge::in_ROI_xmin] = extractor[ftr_ext::sck::extract::out_ROI_xmin];
        merger[ftr_mrg::sck::merge::in_ROI_xmax] = extractor[ftr_ext::sck::extract::out_ROI_xmax];
        merger[ftr_mrg::sck::merge::in_ROI_ymin] = extractor[ftr_ext::sck::extract::out_ROI_ymin];
        merger[ftr_mrg::sck::merge::in_ROI_ymax] = extractor[ftr_ext::sck::extract::out_ROI_ymax];
        merger[ftr_mrg::sck::merge::in_ROI_S] = extractor[ftr_ext::sck::extract::out_ROI_S];
        merger[ftr_mrg::sck::merge::in_ROI_x] = extractor[ftr_ext::sck::extract::out_ROI_x];
        merger[ftr_mrg::sck::merge::in_ROI_y] = extractor[ftr_ext::sck::extract::out_ROI_y];
        merger[ftr_mrg::sck::merge::in_n_ROI] = lsl[ccl::sck::apply::out_n_ROI];
        merger[ftr_mrg::tsk::merge].exec();

        // Step 4 : mise en correspondance
        matcher[knn::sck::match::in_ROI0_id] = delayer_ROI_id[dly::sck::produce::out];
        matcher[knn::sck::match::in_ROI0_x] = delayer_ROI_x[dly::sck::produce::out];
        matcher[knn::sck::match::in_ROI0_y] = delayer_ROI_y[dly::sck::produce::out];
        matcher[knn::sck::match::in_n_ROI0] = delayer_n_ROI[dly::sck::produce::out];
        matcher[knn::sck::match::in_ROI1_id] = merger[ftr_mrg::sck::merge::out_ROI_id];
        matcher[knn::sck::match::in_ROI1_x] = merger[ftr_mrg::sck::merge::out_ROI_x];
        matcher[knn::sck::match::in_ROI1_y] = merger[ftr_mrg::sck::merge::out_ROI_y];
        matcher[knn::sck::match::in_n_ROI1] = merger[ftr_mrg::sck::merge::out_n_ROI];
        matcher[knn::tsk::match].exec();

        // Step 5 : recalage
        motion[ftr_mtn::sck::compute::in_ROI0_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
        motion[ftr_mtn::sck::compute::in_ROI0_x] = delayer_ROI_x[dly::sck::produce::out];
        motion[ftr_mtn::sck::compute::in_ROI0_y] = delayer_ROI_y[dly::sck::produce::out];
        motion[ftr_mtn::sck::compute::in_n_ROI0] = delayer_n_ROI[dly::sck::produce::out];
        motion[ftr_mtn::sck::compute::in_ROI1_x] = merger[ftr_mrg::sck::merge::out_ROI_x];
        motion[ftr_mtn::sck::compute::in_ROI1_y] = merger[ftr_mrg::sck::merge::out_ROI_y];
        motion[ftr_mtn::tsk::compute].exec();

        // Step 6: tracking
        tracking[trk::sck::perform::in_frame] = video[vid::sck::generate::out_frame];
        tracking[trk::sck::perform::in_ROI0_id] = delayer_ROI_id[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_frame] = delayer_ROI_frame[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_xmin] = delayer_ROI_xmin[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_xmax] = delayer_ROI_xmax[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_ymin] = delayer_ROI_ymin[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_ymax] = delayer_ROI_ymax[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_x] = delayer_ROI_x[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_y] = delayer_ROI_y[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_error] = motion[ftr_mtn::sck::compute::out_ROI0_error];
        tracking[trk::sck::perform::in_ROI0_time] = delayer_ROI_time[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_time_motion] = delayer_ROI_time_motion[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_prev_id] = delayer_ROI_prev_id[dly::sck::produce::out];
        tracking[trk::sck::perform::in_ROI0_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
        tracking[trk::sck::perform::in_ROI0_is_extrapolated] = delayer_ROI_is_extrapolated[dly::sck::produce::out];
        tracking[trk::sck::perform::in_n_ROI0] = delayer_n_ROI[dly::sck::produce::out];
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
        tracking[trk::tsk::perform].exec();

        // -- COPIES BEGIN
        delayer_ROI_id[dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_id];
        delayer_ROI_xmin[dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_xmin];
        delayer_ROI_xmax[dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_xmax];
        delayer_ROI_ymin[dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_ymin];
        delayer_ROI_ymax[dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_ymax];
        delayer_ROI_S[dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_S];
        delayer_ROI_x[dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_x];
        delayer_ROI_y[dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_ROI_y];
        delayer_ROI_prev_id[dly::sck::memorize::in] = matcher[knn::sck::match::out_ROI1_prev_id];
        delayer_ROI_frame[dly::sck::memorize::in] = tracking[trk::sck::perform::out_ROI1_frame];
        delayer_ROI_time[dly::sck::memorize::in] = tracking[trk::sck::perform::out_ROI1_time];
        delayer_ROI_time_motion[dly::sck::memorize::in] = tracking[trk::sck::perform::out_ROI1_time_motion];
        delayer_ROI_is_extrapolated[dly::sck::memorize::in] = tracking[trk::sck::perform::out_ROI1_is_extrapolated];
        delayer_n_ROI[dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_n_ROI];

        delayer_ROI_id[dly::tsk::memorize].exec();
        delayer_ROI_xmin[dly::tsk::memorize].exec();
        delayer_ROI_xmax[dly::tsk::memorize].exec();
        delayer_ROI_ymin[dly::tsk::memorize].exec();
        delayer_ROI_ymax[dly::tsk::memorize].exec();
        delayer_ROI_S[dly::tsk::memorize].exec();
        delayer_ROI_x[dly::tsk::memorize].exec();
        delayer_ROI_y[dly::tsk::memorize].exec();
        delayer_ROI_prev_id[dly::tsk::memorize].exec();
        delayer_ROI_frame[dly::tsk::memorize].exec();
        delayer_ROI_time[dly::tsk::memorize].exec();
        delayer_ROI_time_motion[dly::tsk::memorize].exec();
        delayer_ROI_is_extrapolated[dly::tsk::memorize].exec();
        delayer_n_ROI[dly::tsk::memorize].exec();
        // -- COPIES END

        video[vid::tsk::generate].exec();

        // Saving frames
        if (p_out_frames) {
            tools_create_folder(p_out_frames);
            char filename[1024];
            sprintf(filename, "%s/%05u.pgm", p_out_frames, frame);
            tools_save_frame_ui8matrix(filename, (uint8_t**)merger[ftr_mrg::sck::merge::out_img].get_dataptr(), i0, i1,
                                       j0, j1);
        }

        // Saving stats
        if (p_out_stats && n_frames) {
            tools_create_folder(p_out_stats);
            //
            // KPPV_save_asso_conflicts(p_out_stats, frame - 1, matcher.get_data(), ROI_array0, ROI_array1,
            //                          tracking.get_track_array());
            // tools_save_motion(path_motion, theta, tx, ty, frame-1);
            // tools_save_motionExtraction(path_extraction, ROI_array0.data, ROI_array1.data, ROI_array0.size, theta,
            //                             tx, ty, frame-1);
            // tools_save_error(path_error, ROI_array0.data, ROI_array0.size);
        }

        n_frames++;
        real_n_tracks = tracking_count_objects(tracking.get_track_array(), &n_stars, &n_meteors, &n_noise);
        fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\r", n_meteors, n_stars,
                n_noise, real_n_tracks);
        fflush(stderr);
    }
    fprintf(stderr, "\n");

    if (p_out_bb)
        tracking_save_array_BB(p_out_bb, tracking.get_BB_array(), tracking.get_track_array(), MAX_N_FRAMES,
                               p_track_all);
    tracking_print_track_array(stdout, tracking.get_track_array());

    printf("# Statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, real_n_tracks);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
