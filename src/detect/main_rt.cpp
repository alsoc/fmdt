#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <memory>
#include <nrc2.h>
#include <aff3ct-core.hpp>

#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "fmdt/macros.h"

#include "fmdt/CCL_LSL/CCL_LSL.hpp"
#include "fmdt/Features/Features_extractor.hpp"
#include "fmdt/Features/Features_merger_CCL_HI.hpp"
#include "fmdt/Features/Features_motion.hpp"
#include "fmdt/Features/Features_magnitude.hpp"
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
    char* def_p_out_probes = NULL;
    int def_p_video_loop = 1;
    int def_p_ffmpeg_threads = 0;
#ifdef OPENCV_LINK
    char def_p_img_ext[] = "pgm";
#endif

    // help
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
                "  --out-mag           Path to the file containing magnitudes of the tracked objects          [%s]\n",
                def_p_out_mag ? def_p_out_mag : "NULL");
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
                "  --video-buff        Bufferize all the video in global memory before executing the chain        \n");
        fprintf(stderr,
                "  --video-loop        Number of times the video is read in loop                              [%d]\n",
                def_p_video_loop);
        fprintf(stderr,
                "  --ffmpeg-threads    Select the number of threads to use to decode video input (in ffmpeg)  [%d]\n",
                def_p_ffmpeg_threads);
#ifdef OPENCV_LINK
        fprintf(stderr,
                "  --show-id           Show the ROI/CC ids on the ouptut frames                                   \n");
        fprintf(stderr,
                "  --img-ext           Image extension of saved frames ('jpg', 'png', 'tiff', ...)            [%s]\n",
                def_p_img_ext);
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
    const int p_video_buff = args_find(argc, argv, "--video-buff");
    const int p_video_loop = args_find_int_min(argc, argv, "--video-loop", def_p_video_loop, 1);
    const int p_task_stats = args_find(argc, argv, "--task-stats");
    const char* p_out_probes = args_find_char(argc, argv, "--out-probes", def_p_out_probes);
#ifdef OPENCV_LINK
    const int p_show_id = args_find(argc, argv, "--show-id");
    const char* p_img_ext = args_find_char(argc, argv, "--img-ext", def_p_img_ext);
#else
    const int p_show_id = 0;
    const char p_img_ext[] = "pgm";
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
    printf("#  * out-probes     = %s\n", p_out_probes);
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
    printf("#  * video-buff     = %d\n", p_video_buff);
    printf("#  * video-loop     = %d\n", p_video_loop);
    printf("#  * ffmpeg-threads = %d\n", p_ffmpeg_threads);
#ifdef OPENCV_LINK
    printf("#  * show-id        = %d\n", p_show_id);
    printf("#  * img-ext        = %s\n", p_img_ext);
#endif
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
    if (!tools_is_dir(p_in_video) && p_video_buff)
        fprintf(stderr, "(WW) '--video-buff' has no effect when '--in-video' is a video file.\n");
    if (!tools_is_dir(p_in_video) && p_video_loop > 1)
        fprintf(stderr, "(WW) '--video-loop' has no effect when '--in-video' is a video file.\n");
    if (p_ffmpeg_threads && tools_is_dir(p_in_video))
        fprintf(stderr, "(WW) '--ffmpeg-threads' has no effect when '--in-video' is a folder of images\n");
#ifndef FMDT_ENABLE_PIPELINE
    if (p_out_probes)
        fprintf(stderr, "(WW) Using '--out-probes' without pipeline is not very useful...\n");
#endif
#ifdef OPENCV_LINK
    if (p_show_id && !p_out_frames)
        fprintf(stderr, "(WW) '--show-id' has to be combined with the '--out-frames' parameter\n");
#endif

    // -------------------------------- //
    // -- GLOBAL DATA INITIALISATION -- //
    // -------------------------------- //

    tracking_init_global_data();

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    // objects allocation
    const size_t b = 1; // image border
    std::unique_ptr<Video> video;
    std::unique_ptr<Images> images;
    size_t i0, i1, j0, j1;
    if (!tools_is_dir(p_in_video)) {
        video.reset(new Video(p_in_video, p_fra_start, p_fra_end, p_fra_skip, p_ffmpeg_threads, b));
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
    Features_extractor extractor(i0, i1, j0, j1, b, MAX_ROI_SIZE_BEFORE_SHRINK);
    extractor.set_custom_name("Extractor");
    Features_merger_CCL_HI merger(i0, i1, j0, j1, b, p_surface_min, p_surface_max, MAX_ROI_SIZE_BEFORE_SHRINK, MAX_ROI_SIZE);
    merger.set_custom_name("Merger");
    Features_magnitude magnitude(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    magnitude.set_custom_name("Magnitude");
    KNN_matcher matcher(i0, i1, j0, j1, p_k, p_max_dist, p_min_ratio_s, MAX_ROI_SIZE);
    Features_motion motion(MAX_ROI_SIZE);
    motion.set_custom_name("Motion");
    Tracking tracking(p_r_extrapol, p_angle_max, p_diff_dev, p_track_all, p_fra_star_min, p_fra_meteor_min,
                      p_fra_meteor_max, p_out_bb, p_out_mag, p_extrapol_order, p_min_ratio_s, MAX_ROI_SIZE);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_id(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_xmin(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_xmax(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_ymin(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_ymax(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_S(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_Sx(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_Sy(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<float> delayer_ROI_x(MAX_ROI_SIZE, 0.f);
    aff3ct::module::Delayer<float> delayer_ROI_y(MAX_ROI_SIZE, 0.f);
    aff3ct::module::Delayer<uint32_t> delayer_ROI_magnitude(MAX_ROI_SIZE, 0);
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
    delayer_ROI_magnitude.set_custom_name("D<ROI_mag>");
    delayer_n_ROI.set_custom_name("D<n_ROI>");
    Logger_ROI log_ROI(p_out_stats ? p_out_stats : "", MAX_ROI_SIZE, tracking.get_data());
    Logger_KNN log_KNN(p_out_stats ? p_out_stats : "", i0, i1, j0, j1, MAX_ROI_SIZE);
    Logger_motion log_motion(p_out_stats ? p_out_stats : "");
    log_motion.set_custom_name("Logger_motio");
    Logger_track log_track(p_out_stats ? p_out_stats : "", tracking.get_data());
    std::unique_ptr<Logger_frame> log_frame;
    if (p_out_frames)
        log_frame.reset(new Logger_frame(p_out_frames, p_img_ext, p_show_id, i0, i1, j0, j1, b, MAX_ROI_SIZE));

    // create reporters and probes for the real-time probes file
    size_t inter_frame_lvl = 1;
    aff3ct::tools::Reporter_probe rep_fra_stats("Frame Counter", inter_frame_lvl);
    std::unique_ptr<aff3ct::module::Probe<>> prb_fra_id(rep_fra_stats.create_probe_occurrence("ID"));

    aff3ct::tools::Reporter_probe rep_thr_stats("Throughput, latency", "and time", inter_frame_lvl);
    std::unique_ptr<aff3ct::module::Probe<>> prb_thr_thr  (rep_thr_stats.create_probe_throughput("FPS"));
    std::unique_ptr<aff3ct::module::Probe<>> prb_thr_lat  (rep_thr_stats.create_probe_latency   ("LAT")); // only valid for sequence, invalid for pipeline
    std::unique_ptr<aff3ct::module::Probe<>> prb_thr_time (rep_thr_stats.create_probe_time      ("TIME"));

    aff3ct::tools::Reporter_probe rep_timestamp_stats("Timestamps", "(in microseconds) [SX = stage X, B = begin, E = end]", inter_frame_lvl);
    const uint64_t mod = 1000000ul * 60ul * 10; // limit to 10 minutes timestamp
    const size_t probe_buff = 200; // size of the buffer used by the probes to record values
    std::unique_ptr<aff3ct::module::Probe<>>         prb_ts_s1b(rep_timestamp_stats.create_probe_timestamp      ("S1_B", mod,    probe_buff   )); // timestamp stage 1 begin
    std::unique_ptr<aff3ct::module::Probe<>>         prb_ts_s1e(rep_timestamp_stats.create_probe_timestamp      ("S1_E", mod,    probe_buff   )); // timestamp stage 1 end
    std::unique_ptr<aff3ct::module::Probe<uint64_t>> prb_ts_s2b(rep_timestamp_stats.create_probe_value<uint64_t>("S2_B", "(us)", probe_buff, 1)); // timestamp stage 2 begin
    std::unique_ptr<aff3ct::module::Probe<uint64_t>> prb_ts_s2e(rep_timestamp_stats.create_probe_value<uint64_t>("S2_E", "(us)", probe_buff, 1)); // timestamp stage 2 end
    std::unique_ptr<aff3ct::module::Probe<>>         prb_ts_s3b(rep_timestamp_stats.create_probe_timestamp      ("S3_B", mod,    probe_buff   )); // timestamp stage 3 begin
    std::unique_ptr<aff3ct::module::Probe<>>         prb_ts_s3e(rep_timestamp_stats.create_probe_timestamp      ("S3_E", mod,    probe_buff   )); // timestamp stage 3 end

    const std::vector<aff3ct::tools::Reporter*>& reporters = { &rep_fra_stats, &rep_thr_stats, &rep_timestamp_stats };
    aff3ct::tools::Terminal_dump terminal_probes(reporters);

    std::ofstream rt_probes_file;
    if (p_out_probes) {
        rt_probes_file.open(p_out_probes);
        rt_probes_file << "####################" << std::endl;
        rt_probes_file << "# Real-time probes #" << std::endl;
        rt_probes_file << "####################" << std::endl;
        terminal_probes.legend(rt_probes_file);
    }

    // create on-the-fly stateless modules to collect timestamps in the stage 2 (parallel) of the pipeline
    std::unique_ptr<aff3ct::module::Stateless> ts_s2b(new aff3ct::module::Stateless());
    ts_s2b->set_name("Timestamper");
    ts_s2b->set_short_name("Timestamper");
    auto &tsk = ts_s2b->create_task("exec");
    auto ts_out_val = ts_s2b->create_socket_out<uint64_t>(tsk, "out", 1);
    ts_s2b->create_codelet(tsk, [ts_out_val](aff3ct::module::Module &m, aff3ct::runtime::Task &t,
        const size_t frame_id) -> int {
        std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        );
        static_cast<uint64_t*>(t[ts_out_val].get_dataptr())[frame_id] =
            mod ? (uint64_t)us.count() % mod : (uint64_t)us.count();
        return aff3ct::runtime::status_t::SUCCESS;
    });
    std::unique_ptr<aff3ct::module::Stateless> ts_s2e(ts_s2b->clone());
    ts_s2b->set_custom_name("Tsta<S2_B>");
    ts_s2e->set_custom_name("Tsta<S2_E>");

    // ------------------- //
    // -- TASKS BINDING -- //
    // ------------------- //

    if (p_out_probes) {
        if (video)
            (*video)[vid::tsk::generate] = (*prb_ts_s1b)[aff3ct::module::prb::sck::probe_noin::status];
        else
            (*images)[img::tsk::generate] = (*prb_ts_s1b)[aff3ct::module::prb::sck::probe_noin::status];

        (*prb_ts_s1e)[aff3ct::module::prb::tsk::probe] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
        (*ts_s2b)("exec") = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
        (*prb_ts_s2b)[aff3ct::module::prb::sck::probe::in] = (*ts_s2b)["exec::out"];
    }

    // step 1: threshold low
    threshold_min[thr::sck::apply::in_img] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];

    // step 2: CCL/CCA
    lsl[ccl::sck::apply::in_img] = threshold_min[thr::sck::apply::out_img];
    extractor[ftr_ext::sck::extract::in_img] = lsl[ccl::sck::apply::out_labels];
    extractor[ftr_ext::sck::extract::in_n_ROI] = lsl[ccl::sck::apply::out_n_ROI];

    // step 3: hysteresis threshold & surface filtering
    threshold_max[thr::sck::apply::in_img] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    merger[ftr_mrg::sck::merge::in_labels] = lsl[ccl::sck::apply::out_labels];
    merger[ftr_mrg::sck::merge::in_img_HI] = threshold_max[thr::sck::apply::out_img];
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

    // step 3.5 : compute magnitude for each ROI
    magnitude[ftr_mgn::sck::compute::in_img] = video ? (*video)[vid::sck::generate::out_img] : (*images)[img::sck::generate::out_img];
    magnitude[ftr_mgn::sck::compute::in_labels] = merger[ftr_mrg::sck::merge::out_labels];
    magnitude[ftr_mgn::sck::compute::in_ROI_xmin] = merger[ftr_mrg::sck::merge::out_ROI_xmin];
    magnitude[ftr_mgn::sck::compute::in_ROI_xmax] = merger[ftr_mrg::sck::merge::out_ROI_xmax];
    magnitude[ftr_mgn::sck::compute::in_ROI_ymin] = merger[ftr_mrg::sck::merge::out_ROI_ymin];
    magnitude[ftr_mgn::sck::compute::in_ROI_ymax] = merger[ftr_mrg::sck::merge::out_ROI_ymax];
    magnitude[ftr_mgn::sck::compute::in_ROI_S] = merger[ftr_mrg::sck::merge::out_ROI_S];
    magnitude[ftr_mgn::sck::compute::in_n_ROI] = merger[ftr_mrg::sck::merge::out_n_ROI];

    if (p_out_probes) {
        (*ts_s2e)("exec") = merger[ftr_mrg::sck::merge::out_ROI_id];
        (*prb_ts_s2e)[aff3ct::module::prb::sck::probe::in] = (*ts_s2e)["exec::out"];
        (*prb_ts_s3b)[aff3ct::module::prb::tsk::probe] = (*prb_ts_s2e)[aff3ct::module::prb::sck::probe::status];
    }

    // step 3.5 : delayer => save t - 1 ROI statistics
    delayer_ROI_id[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_xmin[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_xmax[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_ymin[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_ymax[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_S[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_Sx[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_Sy[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_x[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_y[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];
    delayer_ROI_magnitude[aff3ct::module::dly::tsk::produce] = magnitude[ftr_mgn::sck::compute::out_ROI_magnitude];
    delayer_n_ROI[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_ROI_id];

    // step 4: k-NN matching
    matcher[knn::sck::match::in_ROI0_id] = delayer_ROI_id[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_ROI0_S] = delayer_ROI_S[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_ROI0_x] = delayer_ROI_x[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_ROI0_y] = delayer_ROI_y[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_n_ROI0] = delayer_n_ROI[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_ROI1_id] = merger[ftr_mrg::sck::merge::out_ROI_id];
    matcher[knn::sck::match::in_ROI1_S] = merger[ftr_mrg::sck::merge::out_ROI_S];
    matcher[knn::sck::match::in_ROI1_x] = merger[ftr_mrg::sck::merge::out_ROI_x];
    matcher[knn::sck::match::in_ROI1_y] = merger[ftr_mrg::sck::merge::out_ROI_y];
    matcher[knn::sck::match::in_n_ROI1] = merger[ftr_mrg::sck::merge::out_n_ROI];

    // step 5: motion estimation
    motion[ftr_mtn::sck::compute::in_ROI0_x] = delayer_ROI_x[aff3ct::module::dly::sck::produce::out];
    motion[ftr_mtn::sck::compute::in_ROI0_y] = delayer_ROI_y[aff3ct::module::dly::sck::produce::out];
    motion[ftr_mtn::sck::compute::in_ROI1_x] = merger[ftr_mrg::sck::merge::out_ROI_x];
    motion[ftr_mtn::sck::compute::in_ROI1_y] = merger[ftr_mrg::sck::merge::out_ROI_y];
    motion[ftr_mtn::sck::compute::in_ROI1_prev_id] = matcher[knn::sck::match::out_ROI1_prev_id];
    motion[ftr_mtn::sck::compute::in_n_ROI1] = merger[ftr_mrg::sck::merge::out_n_ROI];

    // step 6 : tracking
    tracking[trk::sck::perform::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];
    tracking[trk::sck::perform::in_ROI_id] = merger[ftr_mrg::sck::merge::out_ROI_id];
    tracking[trk::sck::perform::in_ROI_xmin] = merger[ftr_mrg::sck::merge::out_ROI_xmin];
    tracking[trk::sck::perform::in_ROI_xmax] = merger[ftr_mrg::sck::merge::out_ROI_xmax];
    tracking[trk::sck::perform::in_ROI_ymin] = merger[ftr_mrg::sck::merge::out_ROI_ymin];
    tracking[trk::sck::perform::in_ROI_ymax] = merger[ftr_mrg::sck::merge::out_ROI_ymax];
    tracking[trk::sck::perform::in_ROI_S] = merger[ftr_mrg::sck::merge::out_ROI_S];
    tracking[trk::sck::perform::in_ROI_x] = merger[ftr_mrg::sck::merge::out_ROI_x];
    tracking[trk::sck::perform::in_ROI_y] = merger[ftr_mrg::sck::merge::out_ROI_y];
    tracking[trk::sck::perform::in_ROI_error] = motion[ftr_mtn::sck::compute::out_ROI1_error];
    tracking[trk::sck::perform::in_ROI_prev_id] = matcher[knn::sck::match::out_ROI1_prev_id];
    tracking[trk::sck::perform::in_ROI_magnitude] = magnitude[ftr_mgn::sck::compute::out_ROI_magnitude];
    tracking[trk::sck::perform::in_n_ROI] = merger[ftr_mrg::sck::merge::out_n_ROI];
    tracking[trk::sck::perform::in_motion_est] = motion[ftr_mtn::sck::compute::out_motion_est2];

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
    delayer_ROI_magnitude[aff3ct::module::dly::sck::memorize::in] = magnitude[ftr_mgn::sck::compute::out_ROI_magnitude];
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
        log_ROI[lgr_roi::sck::write::in_ROI0_magnitude] = delayer_ROI_magnitude[aff3ct::module::dly::sck::produce::out];
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
        log_ROI[lgr_roi::sck::write::in_ROI1_magnitude] = magnitude[ftr_mgn::sck::compute::out_ROI_magnitude];
        log_ROI[lgr_roi::sck::write::in_n_ROI1] = merger[ftr_mrg::sck::merge::out_n_ROI];
        log_ROI[lgr_roi::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];

        log_KNN[lgr_knn::sck::write::in_data_nearest] = matcher[knn::sck::match::out_data_nearest];
        log_KNN[lgr_knn::sck::write::in_data_distances] = matcher[knn::sck::match::out_data_distances];
        log_KNN[lgr_knn::sck::write::in_data_conflicts] = matcher[knn::sck::match::out_data_conflicts];
        log_KNN[lgr_knn::sck::write::in_ROI0_id] = delayer_ROI_id[aff3ct::module::dly::sck::produce::out];
        log_KNN[lgr_knn::sck::write::in_ROI0_next_id] = matcher[knn::sck::match::out_ROI0_next_id];
        log_KNN[lgr_knn::sck::write::in_n_ROI0] = delayer_n_ROI[aff3ct::module::dly::sck::produce::out];
        log_KNN[lgr_knn::sck::write::in_ROI1_dx] = motion[ftr_mtn::sck::compute::out_ROI1_dx];
        log_KNN[lgr_knn::sck::write::in_ROI1_dy] = motion[ftr_mtn::sck::compute::out_ROI1_dy];
        log_KNN[lgr_knn::sck::write::in_ROI1_error] = motion[ftr_mtn::sck::compute::out_ROI1_error];
        log_KNN[lgr_knn::sck::write::in_ROI1_is_moving] = motion[ftr_mtn::sck::compute::out_ROI1_is_moving];
        log_KNN[lgr_knn::sck::write::in_n_ROI1] = merger[ftr_mrg::sck::merge::out_n_ROI];
        log_KNN[lgr_knn::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];

        log_motion[lgr_mtn::sck::write::in_motion_est1] = motion[ftr_mtn::sck::compute::out_motion_est1];
        log_motion[lgr_mtn::sck::write::in_motion_est2] = motion[ftr_mtn::sck::compute::out_motion_est2];
        log_motion[lgr_mtn::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];

        log_track[lgr_trk::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];
    }

    if (p_out_frames) {
        (*log_frame)[lgr_fra::sck::write::in_labels] = merger[ftr_mrg::sck::merge::out_labels];
        (*log_frame)[lgr_fra::sck::write::in_frame] = video ? (*video)[vid::sck::generate::out_frame] : (*images)[img::sck::generate::out_frame];
        (*log_frame)[lgr_fra::sck::write::in_ROI_id] = merger[ftr_mrg::sck::merge::out_ROI_id];
        (*log_frame)[lgr_fra::sck::write::in_ROI_xmax] = merger[ftr_mrg::sck::merge::out_ROI_xmax];
        (*log_frame)[lgr_fra::sck::write::in_ROI_ymin] = merger[ftr_mrg::sck::merge::out_ROI_ymin];
        (*log_frame)[lgr_fra::sck::write::in_ROI_ymax] = merger[ftr_mrg::sck::merge::out_ROI_ymax];
        (*log_frame)[lgr_fra::sck::write::in_n_ROI] = merger[ftr_mrg::sck::merge::out_n_ROI];
    }

    if (p_out_probes) {
        (*prb_fra_id  )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_thr )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_lat )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_time)[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        if (p_out_frames)
            (*prb_ts_s3e)[aff3ct::module::prb::tsk::probe] = (*log_frame)[lgr_fra::sck::write::status];
        else if (p_out_stats)
            (*prb_ts_s3e)[aff3ct::module::prb::tsk::probe] = log_track[lgr_trk::sck::write::status];
        else
            (*prb_ts_s3e)[aff3ct::module::prb::tsk::probe] = (*prb_thr_time)[aff3ct::module::prb::sck::probe_noin::status];
    }

    // --------------------------------- //
    // -- CREATE SEQUENCE OR PIPELINE -- //
    // --------------------------------- //

    // determine the first task in the tasks graph
    aff3ct::runtime::Task* first_task = nullptr;
    if (p_out_probes)
        first_task = &(*prb_ts_s1b)[aff3ct::module::prb::tsk::probe];
    else
        first_task = video ? &(*video)[vid::tsk::generate] : &(*images)[img::tsk::generate];

#ifdef FMDT_ENABLE_PIPELINE
    // pipeline definition with separation stages
    std::vector<std::tuple<std::vector<aff3ct::runtime::Task*>,
                           std::vector<aff3ct::runtime::Task*>,
                           std::vector<aff3ct::runtime::Task*>>> sep_stages;

    if (!p_out_probes) {
        sep_stages =
        { // pipeline stage 1
          std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                          std::vector<aff3ct::runtime::Task*>>(
            { video ? &(*video)[vid::tsk::generate] : &(*images)[img::tsk::generate], },
            { video ? &(*video)[vid::tsk::generate] : &(*images)[img::tsk::generate], },
            { /* no exclusions in this stage */ } ),
          // pipeline stage 2
          std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                          std::vector<aff3ct::runtime::Task*>>(
            { &threshold_min[thr::tsk::apply], &threshold_max[thr::tsk::apply], &magnitude[ftr_mgn::tsk::compute] },
            { &merger[ftr_mrg::tsk::merge], &magnitude[ftr_mgn::tsk::compute]},
            { /* no exclusions in this stage */ } ),
          // pipeline stage 3
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
              &delayer_ROI_magnitude[aff3ct::module::dly::tsk::produce],
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
              &delayer_ROI_magnitude[aff3ct::module::dly::tsk::memorize],
              &delayer_n_ROI[aff3ct::module::dly::tsk::memorize],
              },
            { },
            { /* no exclusions in this stage */ } ),
        };
    } else {
        sep_stages =
        { // pipeline stage 1
          std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                          std::vector<aff3ct::runtime::Task*>>(
            { &(*prb_ts_s1b)[aff3ct::module::prb::tsk::probe], &(*prb_ts_s1e)[aff3ct::module::prb::tsk::probe] },
            { video ? &(*video)[vid::tsk::generate] : &(*images)[img::tsk::generate], },
            { /* no exclusions in this stage */ } ),
          // pipeline stage 2
          std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                          std::vector<aff3ct::runtime::Task*>>(
            { &(*ts_s2b)("exec"), &threshold_min[thr::tsk::apply], &threshold_max[thr::tsk::apply], &magnitude[ftr_mgn::tsk::compute], &(*ts_s2e)("exec") },
            { &merger[ftr_mrg::tsk::merge], &magnitude[ftr_mgn::tsk::compute] },
            { &(*prb_ts_s2b)[aff3ct::module::prb::tsk::probe], &(*prb_ts_s2e)[aff3ct::module::prb::tsk::probe], } ),
          // pipeline stage 3
          std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                          std::vector<aff3ct::runtime::Task*>>(
            { &(*prb_ts_s2b)[aff3ct::module::prb::tsk::probe],
              &(*prb_ts_s2e)[aff3ct::module::prb::tsk::probe],
              &delayer_ROI_id[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_xmin[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_xmax[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_ymin[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_ymax[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_S[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_Sx[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_Sy[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_x[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_y[aff3ct::module::dly::tsk::produce],
              &delayer_ROI_magnitude[aff3ct::module::dly::tsk::produce],
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
              &delayer_ROI_magnitude[aff3ct::module::dly::tsk::memorize],
              &delayer_n_ROI[aff3ct::module::dly::tsk::memorize],
              },
            { },
            { /* no exclusions in this stage */ } ),
        };
    }

    if (p_out_stats) {
        std::get<0>(sep_stages[2]).push_back(&log_ROI[lgr_roi::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_KNN[lgr_knn::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_motion[lgr_mtn::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_track[lgr_trk::tsk::write]);
    }

    if (p_out_frames) {
        std::get<0>(sep_stages[2]).push_back(&(*log_frame)[lgr_fra::tsk::write]);
    }

    aff3ct::runtime::Pipeline sequence_or_pipeline({ first_task }, // first task of the sequence
                                                   sep_stages,
                                                   {
                                                     1, // number of threads in the stage 1
                                                     4, // number of threads in the stage 2
                                                     1, // number of threads in the stage 3
                                                   }, {
                                                     16, // synchronization buffer size between stages 1 and 2
                                                     16, // synchronization buffer size between stages 2 and 3
                                                   }, {
                                                     false, // type of waiting between stages 1 and 2 (true = active, false = passive)
                                                     false, // type of waiting between stages 2 and 3 (true = active, false = passive)
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

    // ---------------------------------- //
    // -- EXECUTE SEQUENCE OR PIPELINE -- //
    // ---------------------------------- //

    std::chrono::time_point<std::chrono::steady_clock> t_start;
    unsigned n_frames = 0;
    std::function<bool(const std::vector<const int*>&)> stop_condition =
        [&tracking, &n_frames, &terminal_probes, &rt_probes_file, &t_start] (const std::vector<const int*>& statuses) {
            if (statuses.back() != nullptr) {
                fprintf(stderr, "(II) Frame n°%4u", n_frames);
                unsigned n_stars = 0, n_meteors = 0, n_noise = 0;
                size_t n_tracks = tracking_count_objects(tracking.get_data()->tracks, &n_stars, &n_meteors, &n_noise);

                auto t_stop = std::chrono::steady_clock::now();
                auto time_duration =
                    (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t_stop - t_start).count();
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
            return aff3ct::tools::Terminal::is_interrupt(); // catch "Ctrl+c" signal interruption
        };

    printf("# The program is running...\n");

    if (p_out_probes) {
        // reset start time to NOW!
        prb_thr_thr->reset();
        prb_thr_lat->reset();
        prb_thr_time->reset();
    }

    t_start = std::chrono::steady_clock::now();
#ifdef FMDT_ENABLE_PIPELINE
    sequence_or_pipeline.exec({
        [] (const std::vector<const int*>& statuses) { return false; }, // stop condition stage 1
        [] (const std::vector<const int*>& statuses) { return false; }, // stop condition stage 2
        stop_condition});                                               // stop condition stage 3
#else
    sequence_or_pipeline.exec(stop_condition);
#endif
    auto t_stop = std::chrono::steady_clock::now();

    // ------------------- //
    // -- PRINT RESULTS -- //
    // ------------------- //

    if (rt_probes_file.is_open())
        terminal_probes.final_report(rt_probes_file);

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
    printf("# -> Processed frames = %4d\n", n_frames);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, (unsigned long)real_n_tracks);
    auto time_duration = (int64_t)std::chrono::duration_cast<std::chrono::microseconds>(t_stop - t_start).count();
    auto time_duration_sec = time_duration * 1e-6;
    printf("# -> Took %6.3f seconds (avg %d FPS)\n", time_duration_sec, (int)(n_frames / time_duration_sec));

    // display the statistics of the tasks (if enabled)
    if (p_task_stats) {
#ifdef FMDT_ENABLE_PIPELINE
        auto stages = sequence_or_pipeline.get_stages();
        for (size_t s = 0; s < stages.size(); s++)
        {
            const int n_threads = stages[s]->get_n_threads();
            std::cout << "#" << std::endl << "# Pipeline stage " << (s + 1) << " (" << n_threads << " thread(s)): "
                      << std::endl;
            aff3ct::tools::Stats::show(stages[s]->get_tasks_per_types(), true, false);
        }
#else
        std::cout << "#" << std::endl;
        aff3ct::tools::Stats::show(sequence_or_pipeline.get_tasks_per_types(), true, false);
#endif
    }

    printf("# End of the program, exiting.\n");

#ifdef FMDT_ENABLE_PIPELINE
    sequence_or_pipeline.unbind_adaptors();
#endif

    return EXIT_SUCCESS;
}
