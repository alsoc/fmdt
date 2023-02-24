#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <nrc2.h>
#include <algorithm>

#include "fmdt/args.h"
#include "fmdt/macros.h"
#include "fmdt/tracking/tracking_global.h"
#include "fmdt/tracking/tracking_io.h"

#include "fmdt/aff3ct_wrapper/CCL/CCL.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_extractor.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI_v2.hpp"
#include "fmdt/aff3ct_wrapper/Motion/Motion.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_magnitude.hpp"
#include "fmdt/aff3ct_wrapper/kNN_matcher/kNN_matcher.hpp"
#include "fmdt/aff3ct_wrapper/Threshold/Threshold.hpp"
#include "fmdt/aff3ct_wrapper/Tracking/Tracking.hpp"
#include "fmdt/aff3ct_wrapper/Video/Video2.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_RoIs.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_kNN.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_motion.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_tracks.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_frame.hpp"

#define ENABLE_PIPELINE

int main(int argc, char** argv) {
    // default values
    char* def_p_vid_in_path = NULL;
    int def_p_vid_in_start = 0;
    int def_p_vid_in_stop = 0;
    int def_p_vid_in_skip = 0;
    int def_p_vid_in_threads = 0;
    char def_p_ccl_impl[16] = "LSLH";
    int def_p_ccl_hyst_lo = 55;
    int def_p_ccl_hyst_hi = 80;
    char* def_p_ccl_fra_path = NULL;
    int def_p_mrp_s_min = 3;
    int def_p_mrp_s_max = 1000;
    int def_p_knn_k = 3;
    int def_p_knn_d = 10;
    float def_p_knn_s = 0.125f;
    int def_p_trk_ext_d = 10;
    int def_p_trk_ext_o = 3;
    float def_p_trk_angle = 20;
    int def_p_trk_star_min = 15;
    int def_p_trk_meteor_min = 3;
    int def_p_trk_meteor_max = 100;
    float def_p_trk_ddev = 4.f;
    char* def_p_trk_bb_path = NULL;
    char* def_p_trk_mag_path = NULL;
    char* def_p_log_path = NULL;

    // help
    if (args_find(argc, argv, "--help,-h")) {
        fprintf(stderr,
                "  --vid-in-path       Path to video file or to an images sequence                            [%s]\n",
                def_p_vid_in_path ? def_p_vid_in_path : "NULL");
        fprintf(stderr,
                "  --vid-in-start      Start frame id (included) in the video                                 [%d]\n",
                def_p_vid_in_start);
        fprintf(stderr,
                "  --vid-in-stop       Stop frame id (included) in the video (if set to 0, read entire video) [%d]\n",
                def_p_vid_in_stop);
        fprintf(stderr,
                "  --vid-in-skip       Number of frames to skip                                               [%d]\n",
                def_p_vid_in_skip);
        fprintf(stderr,
                "  --vid-in-threads    Select the number of threads to use to decode video input (in ffmpeg)  [%d]\n",
                def_p_vid_in_threads);
        fprintf(stderr,
                "  --ccl-impl          Select the CCL implementation to use ('LSLH' or 'LSLM')                [%s]\n",
                def_p_ccl_impl);
        fprintf(stderr,
                "  --ccl-hyst-lo       Minimum light intensity for hysteresis threshold (grayscale [0;255])   [%d]\n",
                def_p_ccl_hyst_lo);
        fprintf(stderr,
                "  --ccl-hyst-hi       Maximum light intensity for hysteresis threshold (grayscale [0;255])   [%d]\n",
                def_p_ccl_hyst_hi);
        fprintf(stderr,
                "  --ccl-fra-path      Path of the files for CC debug frames                                  [%s]\n",
                def_p_ccl_fra_path ? def_p_ccl_fra_path : "NULL");
#ifdef FMDT_OPENCV_LINK
        fprintf(stderr,
                "  --ccl-fra-id        Show the RoI/CC ids on the ouptut CC frames                                \n");
#endif
        fprintf(stderr,
                "  --mrp-s-min         Minimum surface of the CCs in pixels                                   [%d]\n",
                def_p_mrp_s_min);
        fprintf(stderr,
                "  --mrp-s-max         Maxumum surface of the CCs in pixels                                   [%d]\n",
                def_p_mrp_s_max);
        fprintf(stderr,
                "  --knn-k             Maximum number of neighbors considered in k-NN algorithm               [%d]\n",
                def_p_knn_k);
        fprintf(stderr,
                "  --knn-d             Maximum distance in pixels between two images (in k-NN)                [%d]\n",
                def_p_knn_d);
        fprintf(stderr,
                "  --knn-s             Minimum surface ratio to match two CCs in k-NN                         [%f]\n",
                def_p_knn_s);
        fprintf(stderr,
                "  --trk-ext-d         Search radius in pixels for CC extrapolation (piece-wise tracking)     [%d]\n",
                def_p_trk_ext_d);
        fprintf(stderr,
                "  --trk-ext-o         Maximum number of frames to extrapolate (linear) for lost objects      [%d]\n",
                def_p_trk_ext_o);
        fprintf(stderr,
                "  --trk-angle         Tracking max angle between two meteors at t-1 and t (in degree)        [%f]\n",
                def_p_trk_angle);
        fprintf(stderr,
                "  --trk-star-min      Minimum number of frames required to track a star                      [%d]\n",
                def_p_trk_star_min);
        fprintf(stderr,
                "  --trk-meteor-min    Minimum number of frames required to track a meteor                    [%d]\n",
                def_p_trk_meteor_min);
        fprintf(stderr,
                "  --trk-meteor-max    Maximum number of frames required to track a meteor                    [%d]\n",
                def_p_trk_meteor_max);
        fprintf(stderr,
                "  --trk-ddev          Multiplication factor of the standard deviation (CC error has to be        \n");
        fprintf(stderr,
                "                      higher than `ddev` x `std dev` to be considered in movement)           [%f]\n",
                def_p_trk_ddev);
        fprintf(stderr,
                "  --trk-all           Tracks all object types (star, meteor or noise)                            \n");
        fprintf(stderr,
                "  --trk-bb-path       Path to the file containing the bounding boxes (frame by frame)        [%s]\n",
                def_p_trk_bb_path ? def_p_trk_bb_path : "NULL");
        fprintf(stderr,
                "  --trk-mag-path      Path to the file containing magnitudes of the tracked objects          [%s]\n",
                def_p_trk_mag_path ? def_p_trk_mag_path : "NULL");
        fprintf(stderr,
                "  --log-path          Path of the output statistics, only required for debugging purpose     [%s]\n",
                def_p_log_path ? def_p_log_path : "NULL");
        fprintf(stderr,
                "  --rt-stats          Display runtime statistics (executed tasks report)                         \n");
        fprintf(stderr,
                "  --help, -h          This help                                                                  \n");
        exit(1);
    }

    // parse arguments
    const char* p_vid_in_path = args_find_char(argc, argv, "--vid-in-path,--in-video", def_p_vid_in_path);
    const int p_vid_in_start = args_find_int_min(argc, argv, "--vid-in-start,--fra-start", def_p_vid_in_start, 0);
    const int p_vid_in_stop = args_find_int_min(argc, argv, "--vid-in-stop,--fra-end", def_p_vid_in_stop, 0);
    const int p_vid_in_skip = args_find_int_min(argc, argv, "--vid-in-skip,--fra-skip", def_p_vid_in_skip, 0);
    const int p_vid_in_threads = args_find_int_min(argc, argv, "--vid-in-threads,--ffmpeg-threads", def_p_vid_in_threads, 0);
    const char* p_ccl_impl = args_find_char(argc, argv, "--ccl-impl", def_p_ccl_impl);
    const int p_ccl_hyst_lo = args_find_int_min_max(argc, argv, "--ccl-hyst-lo,--light-min", def_p_ccl_hyst_lo, 0, 255);
    const int p_ccl_hyst_hi = args_find_int_min_max(argc, argv, "--ccl-hyst-hi,--light-max", def_p_ccl_hyst_hi, 0, 255);
    const char* p_ccl_fra_path = args_find_char(argc, argv, "--ccl-fra-path,--out-frames", def_p_ccl_fra_path);
#ifdef FMDT_OPENCV_LINK
    const int p_ccl_fra_id = args_find(argc, argv, "--ccl-fra-id,--show-id");
#else
    const int p_ccl_fra_id = 0;
#endif
    const int p_mrp_s_min = args_find_int_min(argc, argv, "--mrp-s-min,--surface-min", def_p_mrp_s_min, 0);
    const int p_mrp_s_max = args_find_int_min(argc, argv, "--mrp-s-max,--surface-max", def_p_mrp_s_max, 0);
    const int p_knn_k = args_find_int_min(argc, argv, "--knn-k,-k", def_p_knn_k, 0);
    const int p_knn_d = args_find_int_min(argc, argv, "--knn-d,--max-dist", def_p_knn_d, 0);
    const float p_knn_s = args_find_float_min_max(argc, argv, "--knn-s,--min-ratio-s", def_p_knn_s, 0.f, 1.f);
    const int p_trk_ext_d = args_find_int_min(argc, argv, "--trk-ext-d,--r-extrapol", def_p_trk_ext_d, 0);
    const int p_trk_ext_o = args_find_int_min_max(argc, argv, "--trk-ext-o,--extrapol-order", def_p_trk_ext_o, 0, 255);
    const float p_trk_angle = args_find_float_min_max(argc, argv, "--trk-angle,--angle-max", def_p_trk_angle, 0.f, 360.f);
    const int p_trk_star_min = args_find_int_min(argc, argv, "--trk-star-min,--fra-star-min", def_p_trk_star_min, 2);
    const int p_trk_meteor_min = args_find_int_min(argc, argv, "--trk-meteor-min,--fra-meteor-min", def_p_trk_meteor_min, 2);
    const int p_trk_meteor_max = args_find_int_min(argc, argv, "--trk-meteor-max,--fra-meteor-max", def_p_trk_meteor_max, 2);
    const float p_trk_ddev = args_find_float_min(argc, argv, "--trk-ddev,--diff-dev", def_p_trk_ddev, 0.f);
    const int p_trk_all = args_find(argc, argv, "--trk-all,--track-all");
    const char* p_trk_bb_path = args_find_char(argc, argv, "--trk-bb-path,--out-bb", def_p_trk_bb_path);
    const char* p_trk_mag_path = args_find_char(argc, argv, "--trk-mag-path,--out-mag", def_p_trk_mag_path);
    const char* p_log_path = args_find_char(argc, argv, "--log-path,--out-stats", def_p_log_path);
    const int p_task_stats = args_find(argc, argv, "--rt-stats,--task-stats");

    // heading display
    printf("#  ---------------------\n");
    printf("# |          ----*      |\n");
    printf("# | --* FMDT-DETECT --* |\n");
    printf("# |   -------*          |\n");
    printf("#  ---------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * vid-in-path    = %s\n", p_vid_in_path);
    printf("#  * vid-in-start   = %d\n", p_vid_in_start);
    printf("#  * vid-in-stop    = %d\n", p_vid_in_stop);
    printf("#  * vid-in-skip    = %d\n", p_vid_in_skip);
    printf("#  * vid-in-threads = %d\n", p_vid_in_threads);
    printf("#  * ccl-impl       = %s\n", p_ccl_impl);
    printf("#  * ccl-hyst-lo    = %d\n", p_ccl_hyst_lo);
    printf("#  * ccl-hyst-hi    = %d\n", p_ccl_hyst_hi);
    printf("#  * ccl-fra-path   = %s\n", p_ccl_fra_path);
#ifdef FMDT_OPENCV_LINK
    printf("#  * ccl-fra-id     = %d\n", p_ccl_fra_id);
#endif
    printf("#  * mrp-s-min      = %d\n", p_mrp_s_min);
    printf("#  * mrp-s-max      = %d\n", p_mrp_s_max);
    printf("#  * knn-k          = %d\n", p_knn_k);
    printf("#  * knn-d          = %d\n", p_knn_d);
    printf("#  * knn-s          = %1.3f\n", p_knn_s);
    printf("#  * trk-ext-d      = %d\n", p_trk_ext_d);
    printf("#  * trk-ext-o      = %d\n", p_trk_ext_o);
    printf("#  * trk-angle      = %f\n", p_trk_angle);
    printf("#  * trk-star-min   = %d\n", p_trk_star_min);
    printf("#  * trk-meteor-min = %d\n", p_trk_meteor_min);
    printf("#  * trk-meteor-max = %d\n", p_trk_meteor_max);
    printf("#  * trk-ddev       = %4.2f\n", p_trk_ddev);
    printf("#  * trk-all        = %d\n", p_trk_all);
    printf("#  * trk-bb-path    = %s\n", p_trk_bb_path);
    printf("#  * trk-mag-path   = %s\n", p_trk_mag_path);
    printf("#  * log-path       = %s\n", p_log_path);
    printf("#  * rt-stats       = %d\n", p_task_stats);
    printf("#\n");
#ifdef FMDT_ENABLE_PIPELINE
    printf("#  * Runtime mode   = Pipeline\n");
#else
    printf("#  * Runtime mode   = Sequence\n");
#endif
    printf("#\n");

    // arguments checking
    if (!p_vid_in_path) {
        fprintf(stderr, "(EE) '--vid-in-path' is missing\n");
        exit(1);
    }
    if (p_trk_meteor_max < p_trk_meteor_min) {
        fprintf(stderr, "(EE) '--trk-meteor-max' has to be bigger than '--trk-meteor-min'\n");
        exit(1);
    }
    if (p_vid_in_stop && p_vid_in_stop < p_vid_in_start) {
        fprintf(stderr, "(EE) '--vid-in-stop' has to be higher than '--vid-in-start'\n");
        exit(1);
    }
    if (p_ccl_hyst_lo > p_ccl_hyst_hi) {
        fprintf(stderr, "(EE) '--ccl-hyst-hi' has to be higher than '--ccl-hyst-lo'\n");
        exit(1);
    }
#ifdef FMDT_OPENCV_LINK
    if (p_ccl_fra_id && !p_ccl_fra_path)
        fprintf(stderr, "(WW) '--ccl-fra-id' has to be combined with the '--ccl-fra-path' parameter\n");
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
    Video2 video(p_vid_in_path, p_vid_in_start, p_vid_in_stop, p_vid_in_skip, 0, p_vid_in_threads, b);
    const size_t i0 = video.get_i0();
    const size_t i1 = video.get_i1();
    const size_t j0 = video.get_j0();
    const size_t j1 = video.get_j1();
    
    Threshold threshold_min0(i0, i1, j0, j1, b, p_ccl_hyst_lo);
    threshold_min0.set_custom_name("Thr0<min>");
    Threshold threshold_max0(i0, i1, j0, j1, b, p_ccl_hyst_hi);
    threshold_max0.set_custom_name("Thr0<max>");
    CCL ccl0(i0, i1, j0, j1, b, CCL_str_to_enum(p_ccl_impl));
    ccl0.set_custom_name("CCL0");
    Features_extractor extractor0(i0, i1, j0, j1, b, MAX_ROI_SIZE_BEFORE_SHRINK);
    extractor0.set_custom_name("Extractor0");
    Features_merger_CCL_HI_v2 merger0(i0, i1, j0, j1, b, p_mrp_s_min, p_mrp_s_max, MAX_ROI_SIZE_BEFORE_SHRINK,
                                      MAX_ROI_SIZE);
    merger0.set_custom_name("Merger0");
    Features_magnitude magnitude0(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    magnitude0.set_custom_name("Magnitude0");

    Threshold threshold_min1(i0, i1, j0, j1, b, p_ccl_hyst_lo);
    threshold_min1.set_custom_name("Thr1<min>");
    Threshold threshold_max1(i0, i1, j0, j1, b, p_ccl_hyst_hi);
    threshold_max1.set_custom_name("Thr1<max>");
    CCL ccl1(i0, i1, j0, j1, b, CCL_str_to_enum(p_ccl_impl));
    ccl1.set_custom_name("CCL1");
    Features_extractor extractor1(i0, i1, j0, j1, b, MAX_ROI_SIZE_BEFORE_SHRINK);
    extractor1.set_custom_name("Extractor1");
    Features_merger_CCL_HI_v2 merger1(i0, i1, j0, j1, b, p_mrp_s_min, p_mrp_s_max, MAX_ROI_SIZE_BEFORE_SHRINK,
                                      MAX_ROI_SIZE);
    merger1.set_custom_name("Merger1");
    Features_magnitude magnitude1(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    magnitude1.set_custom_name("Magnitude1");

    kNN_matcher matcher(p_knn_k, p_knn_d, p_knn_s, MAX_ROI_SIZE);
    Motion motion(MAX_ROI_SIZE);
    motion.set_custom_name("Motion");
    Tracking tracking(p_trk_ext_d, p_trk_angle, p_trk_ddev, p_trk_all, p_trk_star_min, p_trk_meteor_min,
                      p_trk_meteor_max, p_trk_bb_path, p_trk_mag_path, p_trk_ext_o, p_knn_s, MAX_ROI_SIZE);
    Logger_RoIs log_RoIs(p_log_path ? p_log_path : "", p_vid_in_start, p_vid_in_skip, MAX_ROI_SIZE, tracking.get_data());
    Logger_kNN log_kNN(p_log_path ? p_log_path : "", p_vid_in_start, MAX_ROI_SIZE);
    Logger_motion log_motion(p_log_path ? p_log_path : "", p_vid_in_start);
    log_motion.set_custom_name("Logger_motio");
    Logger_tracks log_track(p_log_path ? p_log_path : "", p_vid_in_start, tracking.get_data());
    std::unique_ptr<Logger_frame> log_frame;
    if (p_ccl_fra_path)
        log_frame.reset(new Logger_frame(p_ccl_fra_path, p_vid_in_start, p_ccl_fra_id, i0, i1, j0, j1, b, MAX_ROI_SIZE));

    // ------------------- //
    // -- TASKS BINDING -- //
    // ------------------- //

    // Step 1 : seuillage low/high
    threshold_min0[thr::sck::apply::in_img] = video[vid2::sck::generate::out_img0];
    threshold_max0[thr::sck::apply::in_img] = video[vid2::sck::generate::out_img0];
    threshold_min1[thr::sck::apply::in_img] = video[vid2::sck::generate::out_img1];
    threshold_max1[thr::sck::apply::in_img] = video[vid2::sck::generate::out_img1];
    
    // Step 2 : ECC/ACC
    ccl0[ccl::sck::apply::in_img] = threshold_min0[thr::sck::apply::out_img];
    extractor0[ftr_ext::sck::extract::in_img] = ccl0[ccl::sck::apply::out_labels];
    extractor0[ftr_ext::sck::extract::in_n_RoIs] = ccl0[ccl::sck::apply::out_n_RoIs];

    ccl1[ccl::sck::apply::in_img] = threshold_min1[thr::sck::apply::out_img];
    extractor1[ftr_ext::sck::extract::in_img] = ccl1[ccl::sck::apply::out_labels];
    extractor1[ftr_ext::sck::extract::in_n_RoIs] = ccl1[ccl::sck::apply::out_n_RoIs];

    // Step 3 : seuillage hysteresis && filter surface
    merger0[ftr_mrg2::sck::merge::in_labels] = ccl0[ccl::sck::apply::out_labels];
    merger0[ftr_mrg2::sck::merge::in_img_HI] = threshold_max0[thr::sck::apply::out_img];
    merger0[ftr_mrg2::sck::merge::in_RoIs_id] = extractor0[ftr_ext::sck::extract::out_RoIs_id];
    merger0[ftr_mrg2::sck::merge::in_RoIs_xmin] = extractor0[ftr_ext::sck::extract::out_RoIs_xmin];
    merger0[ftr_mrg2::sck::merge::in_RoIs_xmax] = extractor0[ftr_ext::sck::extract::out_RoIs_xmax];
    merger0[ftr_mrg2::sck::merge::in_RoIs_ymin] = extractor0[ftr_ext::sck::extract::out_RoIs_ymin];
    merger0[ftr_mrg2::sck::merge::in_RoIs_ymax] = extractor0[ftr_ext::sck::extract::out_RoIs_ymax];
    merger0[ftr_mrg2::sck::merge::in_RoIs_S] = extractor0[ftr_ext::sck::extract::out_RoIs_S];
    merger0[ftr_mrg2::sck::merge::in_RoIs_Sx] = extractor0[ftr_ext::sck::extract::out_RoIs_Sx];
    merger0[ftr_mrg2::sck::merge::in_RoIs_Sy] = extractor0[ftr_ext::sck::extract::out_RoIs_Sy];
    merger0[ftr_mrg2::sck::merge::in_RoIs_x] = extractor0[ftr_ext::sck::extract::out_RoIs_x];
    merger0[ftr_mrg2::sck::merge::in_RoIs_y] = extractor0[ftr_ext::sck::extract::out_RoIs_y];
    merger0[ftr_mrg2::sck::merge::in_n_RoIs] = ccl0[ccl::sck::apply::out_n_RoIs];

    merger1[ftr_mrg2::sck::merge::in_labels] = ccl1[ccl::sck::apply::out_labels];
    merger1[ftr_mrg2::sck::merge::in_img_HI] = threshold_max1[thr::sck::apply::out_img];
    merger1[ftr_mrg2::sck::merge::in_RoIs_id] = extractor1[ftr_ext::sck::extract::out_RoIs_id];
    merger1[ftr_mrg2::sck::merge::in_RoIs_xmin] = extractor1[ftr_ext::sck::extract::out_RoIs_xmin];
    merger1[ftr_mrg2::sck::merge::in_RoIs_xmax] = extractor1[ftr_ext::sck::extract::out_RoIs_xmax];
    merger1[ftr_mrg2::sck::merge::in_RoIs_ymin] = extractor1[ftr_ext::sck::extract::out_RoIs_ymin];
    merger1[ftr_mrg2::sck::merge::in_RoIs_ymax] = extractor1[ftr_ext::sck::extract::out_RoIs_ymax];
    merger1[ftr_mrg2::sck::merge::in_RoIs_S] = extractor1[ftr_ext::sck::extract::out_RoIs_S];
    merger1[ftr_mrg2::sck::merge::in_RoIs_Sx] = extractor1[ftr_ext::sck::extract::out_RoIs_Sx];
    merger1[ftr_mrg2::sck::merge::in_RoIs_Sy] = extractor1[ftr_ext::sck::extract::out_RoIs_Sy];
    merger1[ftr_mrg2::sck::merge::in_RoIs_x] = extractor1[ftr_ext::sck::extract::out_RoIs_x];
    merger1[ftr_mrg2::sck::merge::in_RoIs_y] = extractor1[ftr_ext::sck::extract::out_RoIs_y];
    merger1[ftr_mrg2::sck::merge::in_n_RoIs] = ccl1[ccl::sck::apply::out_n_RoIs];

    // Step 3.5 : calcul de la magnitude pour chaque RoI
    magnitude0[ftr_mgn::sck::compute::in_img] = video[vid2::sck::generate::out_img0];
    magnitude0[ftr_mgn::sck::compute::in_labels] = merger0[ftr_mrg2::sck::merge::out_labels];
    magnitude0[ftr_mgn::sck::compute::in_RoIs_xmin] = merger0[ftr_mrg2::sck::merge::out_RoIs_xmin];
    magnitude0[ftr_mgn::sck::compute::in_RoIs_xmax] = merger0[ftr_mrg2::sck::merge::out_RoIs_xmax];
    magnitude0[ftr_mgn::sck::compute::in_RoIs_ymin] = merger0[ftr_mrg2::sck::merge::out_RoIs_ymin];
    magnitude0[ftr_mgn::sck::compute::in_RoIs_ymax] = merger0[ftr_mrg2::sck::merge::out_RoIs_ymax];
    magnitude0[ftr_mgn::sck::compute::in_RoIs_S] = merger0[ftr_mrg2::sck::merge::out_RoIs_S];
    magnitude0[ftr_mgn::sck::compute::in_n_RoIs] = merger0[ftr_mrg2::sck::merge::out_n_RoIs];

    magnitude1[ftr_mgn::sck::compute::in_img] = video[vid2::sck::generate::out_img1];
    magnitude1[ftr_mgn::sck::compute::in_labels] = merger1[ftr_mrg2::sck::merge::out_labels];
    magnitude1[ftr_mgn::sck::compute::in_RoIs_xmin] = merger1[ftr_mrg2::sck::merge::out_RoIs_xmin];
    magnitude1[ftr_mgn::sck::compute::in_RoIs_xmax] = merger1[ftr_mrg2::sck::merge::out_RoIs_xmax];
    magnitude1[ftr_mgn::sck::compute::in_RoIs_ymin] = merger1[ftr_mrg2::sck::merge::out_RoIs_ymin];
    magnitude1[ftr_mgn::sck::compute::in_RoIs_ymax] = merger1[ftr_mrg2::sck::merge::out_RoIs_ymax];
    magnitude1[ftr_mgn::sck::compute::in_RoIs_S] = merger1[ftr_mrg2::sck::merge::out_RoIs_S];
    magnitude1[ftr_mgn::sck::compute::in_n_RoIs] = merger1[ftr_mrg2::sck::merge::out_n_RoIs];

    // Step 4 : mise en correspondance
    matcher[knn::sck::match::in_RoIs0_id] = merger0[ftr_mrg2::sck::merge::out_RoIs_id];
    matcher[knn::sck::match::in_RoIs0_S] = merger0[ftr_mrg2::sck::merge::out_RoIs_S];
    matcher[knn::sck::match::in_RoIs0_x] = merger0[ftr_mrg2::sck::merge::out_RoIs_x];
    matcher[knn::sck::match::in_RoIs0_y] = merger0[ftr_mrg2::sck::merge::out_RoIs_y];
    matcher[knn::sck::match::in_n_RoIs0] = merger0[ftr_mrg2::sck::merge::out_n_RoIs];
    matcher[knn::sck::match::in_RoIs1_id] = merger1[ftr_mrg2::sck::merge::out_RoIs_id];
    matcher[knn::sck::match::in_RoIs1_S] = merger1[ftr_mrg2::sck::merge::out_RoIs_S];
    matcher[knn::sck::match::in_RoIs1_x] = merger1[ftr_mrg2::sck::merge::out_RoIs_x];
    matcher[knn::sck::match::in_RoIs1_y] = merger1[ftr_mrg2::sck::merge::out_RoIs_y];
    matcher[knn::sck::match::in_n_RoIs1] = merger1[ftr_mrg2::sck::merge::out_n_RoIs];

    // Step 5 : recalage
    motion[mtn::sck::compute::in_RoIs0_x] = merger0[ftr_mrg2::sck::merge::out_RoIs_x];
    motion[mtn::sck::compute::in_RoIs0_y] = merger0[ftr_mrg2::sck::merge::out_RoIs_y];
    motion[mtn::sck::compute::in_RoIs1_x] = merger1[ftr_mrg2::sck::merge::out_RoIs_x];
    motion[mtn::sck::compute::in_RoIs1_y] = merger1[ftr_mrg2::sck::merge::out_RoIs_y];
    motion[mtn::sck::compute::in_RoIs1_prev_id] = matcher[knn::sck::match::out_RoIs1_prev_id];
    motion[mtn::sck::compute::in_n_RoIs1] = merger1[ftr_mrg2::sck::merge::out_n_RoIs];

    // Step 6 : tracking
    tracking[trk::sck::perform::in_frame] = video[vid2::sck::generate::out_frame];
    tracking[trk::sck::perform::in_RoIs_id] = merger1[ftr_mrg2::sck::merge::out_RoIs_id];
    tracking[trk::sck::perform::in_RoIs_xmin] = merger1[ftr_mrg2::sck::merge::out_RoIs_xmin];
    tracking[trk::sck::perform::in_RoIs_xmax] = merger1[ftr_mrg2::sck::merge::out_RoIs_xmax];
    tracking[trk::sck::perform::in_RoIs_ymin] = merger1[ftr_mrg2::sck::merge::out_RoIs_ymin];
    tracking[trk::sck::perform::in_RoIs_ymax] = merger1[ftr_mrg2::sck::merge::out_RoIs_ymax];
    tracking[trk::sck::perform::in_RoIs_S] = merger1[ftr_mrg2::sck::merge::out_RoIs_S];
    tracking[trk::sck::perform::in_RoIs_x] = merger1[ftr_mrg2::sck::merge::out_RoIs_x];
    tracking[trk::sck::perform::in_RoIs_y] = merger1[ftr_mrg2::sck::merge::out_RoIs_y];
    tracking[trk::sck::perform::in_RoIs_error] = motion[mtn::sck::compute::out_RoIs1_error];
    tracking[trk::sck::perform::in_RoIs_prev_id] = matcher[knn::sck::match::out_RoIs1_prev_id];
    tracking[trk::sck::perform::in_RoIs_magnitude] = magnitude1[ftr_mgn::sck::compute::out_RoIs_magnitude];
    tracking[trk::sck::perform::in_n_RoIs] = merger1[ftr_mrg2::sck::merge::out_n_RoIs];
    tracking[trk::sck::perform::in_motion_est] = motion[mtn::sck::compute::out_motion_est2];

    if (p_ccl_fra_path) {
        (*log_frame)[lgr_fra::sck::write::in_labels] = merger1[ftr_mrg2::sck::merge::out_labels];
        (*log_frame)[lgr_fra::sck::write::in_RoIs_id] = merger1[ftr_mrg2::sck::merge::out_RoIs_id];
        (*log_frame)[lgr_fra::sck::write::in_RoIs_xmin] = merger1[ftr_mrg2::sck::merge::out_RoIs_xmin];
        (*log_frame)[lgr_fra::sck::write::in_RoIs_xmax] = merger1[ftr_mrg2::sck::merge::out_RoIs_xmax];
        (*log_frame)[lgr_fra::sck::write::in_RoIs_ymin] = merger1[ftr_mrg2::sck::merge::out_RoIs_ymin];
        (*log_frame)[lgr_fra::sck::write::in_RoIs_ymax] = merger1[ftr_mrg2::sck::merge::out_RoIs_ymax];
        (*log_frame)[lgr_fra::sck::write::in_n_RoIs] = merger1[ftr_mrg2::sck::merge::out_n_RoIs];
    }

    if (p_log_path) {
        log_RoIs[lgr_roi::sck::write::in_RoIs0_id] = merger0[ftr_mrg2::sck::merge::out_RoIs_id];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_xmin] = merger0[ftr_mrg2::sck::merge::out_RoIs_xmin];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_xmax] = merger0[ftr_mrg2::sck::merge::out_RoIs_xmax];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_ymin] = merger0[ftr_mrg2::sck::merge::out_RoIs_ymin];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_ymax] = merger0[ftr_mrg2::sck::merge::out_RoIs_ymax];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_S] = merger0[ftr_mrg2::sck::merge::out_RoIs_S];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_Sx] = merger0[ftr_mrg2::sck::merge::out_RoIs_Sx];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_Sy] = merger0[ftr_mrg2::sck::merge::out_RoIs_Sy];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_x] = merger0[ftr_mrg2::sck::merge::out_RoIs_x];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_y] = merger0[ftr_mrg2::sck::merge::out_RoIs_y];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_magnitude] = magnitude0[ftr_mgn::sck::compute::out_RoIs_magnitude];
        log_RoIs[lgr_roi::sck::write::in_n_RoIs0] = merger0[ftr_mrg2::sck::merge::out_n_RoIs];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_id] = merger1[ftr_mrg2::sck::merge::out_RoIs_id];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_xmin] = merger1[ftr_mrg2::sck::merge::out_RoIs_xmin];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_xmax] = merger1[ftr_mrg2::sck::merge::out_RoIs_xmax];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_ymin] = merger1[ftr_mrg2::sck::merge::out_RoIs_ymin];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_ymax] = merger1[ftr_mrg2::sck::merge::out_RoIs_ymax];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_S] = merger1[ftr_mrg2::sck::merge::out_RoIs_S];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_Sx] = merger1[ftr_mrg2::sck::merge::out_RoIs_Sx];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_Sy] = merger1[ftr_mrg2::sck::merge::out_RoIs_Sy];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_x] = merger1[ftr_mrg2::sck::merge::out_RoIs_x];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_y] = merger1[ftr_mrg2::sck::merge::out_RoIs_y];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_magnitude] = magnitude1[ftr_mgn::sck::compute::out_RoIs_magnitude];
        log_RoIs[lgr_roi::sck::write::in_n_RoIs1] = merger1[ftr_mrg2::sck::merge::out_n_RoIs];
        log_RoIs[lgr_roi::sck::write::in_frame] = video[vid2::sck::generate::out_frame];

        log_kNN[lgr_knn::sck::write::in_data_nearest] = matcher[knn::sck::match::out_data_nearest];
        log_kNN[lgr_knn::sck::write::in_data_distances] = matcher[knn::sck::match::out_data_distances];
        log_kNN[lgr_knn::sck::write::in_RoIs0_id] = merger0[ftr_mrg2::sck::merge::out_RoIs_id];
        log_kNN[lgr_knn::sck::write::in_RoIs0_next_id] = matcher[knn::sck::match::out_RoIs0_next_id];
        log_kNN[lgr_knn::sck::write::in_n_RoIs0] = merger0[ftr_mrg2::sck::merge::out_n_RoIs];
        log_kNN[lgr_knn::sck::write::in_RoIs1_dx] = motion[mtn::sck::compute::out_RoIs1_dx];
        log_kNN[lgr_knn::sck::write::in_RoIs1_dy] = motion[mtn::sck::compute::out_RoIs1_dy];
        log_kNN[lgr_knn::sck::write::in_RoIs1_error] = motion[mtn::sck::compute::out_RoIs1_error];
        log_kNN[lgr_knn::sck::write::in_RoIs1_is_moving] = motion[mtn::sck::compute::out_RoIs1_is_moving];
        log_kNN[lgr_knn::sck::write::in_n_RoIs1] = merger1[ftr_mrg2::sck::merge::out_n_RoIs];
        log_kNN[lgr_knn::sck::write::in_frame] = video[vid2::sck::generate::out_frame];

        log_motion[lgr_mtn::sck::write::in_motion_est1] = motion[mtn::sck::compute::out_motion_est1];
        log_motion[lgr_mtn::sck::write::in_motion_est2] = motion[mtn::sck::compute::out_motion_est2];
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
        { &threshold_min0[thr::tsk::apply], &threshold_max0[thr::tsk::apply], &threshold_min1[thr::tsk::apply],
          &threshold_max1[thr::tsk::apply], &magnitude0[ftr_mgn::tsk::compute],
          &magnitude1[ftr_mgn::tsk::compute] },
        { &merger0[ftr_mrg2::tsk::merge], &merger1[ftr_mrg2::tsk::merge], },
        { } ),
      // pipeline stage 2
      std::make_tuple<std::vector<aff3ct::runtime::Task*>, std::vector<aff3ct::runtime::Task*>,
                      std::vector<aff3ct::runtime::Task*>>(
        { 
          &matcher[knn::tsk::match],
          &motion[mtn::tsk::compute],
          &tracking[trk::tsk::perform],
          },
        { },
        { /* no exclusions in this stage */ } ),
    };

    if (p_log_path) {
        std::get<0>(sep_stages[2]).push_back(&log_RoIs[lgr_roi::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_kNN[lgr_knn::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_motion[lgr_mtn::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_track[lgr_trk::tsk::write]);
    }

    if (p_ccl_fra_path) {
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
    if (p_trk_bb_path) {
        FILE* f = fopen(p_trk_bb_path, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_trk_bb_path);
            exit(1);
        }
        tracking_BBs_write(f, tracking.get_BBs(), tracking.get_data()->tracks);
        fclose(f);
    }

    if (p_trk_mag_path) {
        FILE* f = fopen(p_trk_mag_path, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_trk_bb_path);
            exit(1);
        }
        tracking_tracks_magnitudes_write(f, tracking.get_data()->tracks);
        fclose(f);
    }
    tracking_tracks_write(stdout, tracking.get_data()->tracks);

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
