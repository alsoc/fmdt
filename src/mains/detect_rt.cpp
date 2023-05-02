#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <memory>
#include <nrc2.h>
#include <vec.h>
#include <aff3ct-core.hpp>

#include "fmdt/args.h"
#include "fmdt/macros.h"
#include "fmdt/tools.h"
#include "fmdt/tools.hpp"
#include "fmdt/tracking/tracking_global.h"
#include "fmdt/tracking/tracking_io.h"
#include "fmdt/version.h"

#include "fmdt/aff3ct_wrapper/CCL_LSL/CCL_LSL.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_extractor.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI.hpp"
#include "fmdt/aff3ct_wrapper/Motion/Motion.hpp"
#include "fmdt/aff3ct_wrapper/Features/Features_magnitude.hpp"
#include "fmdt/aff3ct_wrapper/kNN_matcher/kNN_matcher.hpp"
#include "fmdt/aff3ct_wrapper/Threshold/Threshold.hpp"
#include "fmdt/aff3ct_wrapper/Tracking/Tracking.hpp"
#include "fmdt/aff3ct_wrapper/Video/Video.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_RoIs.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_kNN.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_motion.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_tracks.hpp"
#include "fmdt/aff3ct_wrapper/Logger/Logger_frame.hpp"

// Do not use this define anymore!! NOW it is set in the CMakeFile :-)
// #define FMDT_ENABLE_PIPELINE

int main(int argc, char** argv) {
    // default values
    char* def_p_vid_in_path = NULL;
    int def_p_vid_in_start = 0;
    int def_p_vid_in_stop = 0;
    int def_p_vid_in_skip = 0;
    int def_p_vid_in_loop = 1;
    int def_p_vid_in_threads = 0;
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
    char* def_p_out_probes = NULL;
#ifdef FMDT_ENABLE_PIPELINE
    char def_p_pip_threads[50] = {"[1,4,1]"};
    char def_p_pip_sync[50] = {"[1,1]"};
    char def_p_pip_wait[50] = {"[0,0]"};
    char def_p_pip_pin[50] = {"[0,0,0]"};
    char def_p_pip_pin_vals[50] = {"[[0],[0],[0]]"};
#endif
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
                "  --vid-in-buff       Bufferize all the video in global memory before executing the chain        \n");
        fprintf(stderr,
                "  --vid-in-loop       Number of times the video is read in loop                              [%d]\n",
                def_p_vid_in_loop);
        fprintf(stderr,
                "  --vid-in-threads    Select the number of threads to use to decode video input (in ffmpeg)  [%d]\n",
                def_p_vid_in_threads);
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
                "  --mrp-s-max         Maximum surface of the CCs in pixels                                   [%d]\n",
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
                "  --rt-prb-path       Path of the output probe vales, only required for benchmarking purpose [%s]\n",
                def_p_out_probes ? def_p_out_probes : "NULL");
#ifdef FMDT_ENABLE_PIPELINE
        fprintf(stderr,
                "  --pip-threads       Number of threads for each stage of the pipeline                       [%s]\n",
                def_p_pip_threads); 
        fprintf(stderr,
                "  --pip-sync          Synchronization buffer size between two consecutive pipeline stages    [%s]\n",
                def_p_pip_sync); 
        fprintf(stderr,
                "  --pip-wait          Type of waiting between stages (1 = active, 0 = passive)               [%s]\n",
                def_p_pip_wait);
        fprintf(stderr,
                "  --pip-pin           Enable pinning of threads for each stage of the pipeline               [%s]\n",
                def_p_pip_pin); 
        fprintf(stderr,
                "  --pip-pin-vals      Explicit pinning of threads (has no effect if --pip-pin == 0)          [%s]\n",
                def_p_pip_pin_vals);
#endif
        fprintf(stderr,
                "  --help, -h          This help                                                                  \n");
        fprintf(stderr,
                "  --version, -v       Print the version                                                          \n");
        exit(1);
    }

    // version
    if (args_find(argc, argv, "--version,-v")) {
#ifdef FMDT_ENABLE_PIPELINE
        version_print("detect-rt-pip");
#else
        version_print("detect-rt-seq");
#endif
        exit(0);
    }

    // parse arguments
    const char* p_vid_in_path = args_find_char(argc, argv, "--vid-in-path,--in-video", def_p_vid_in_path);
    const int p_vid_in_start = args_find_int_min(argc, argv, "--vid-in-start,--fra-start", def_p_vid_in_start, 0);
    const int p_vid_in_stop = args_find_int_min(argc, argv, "--vid-in-stop,--fra-end", def_p_vid_in_stop, 0);
    const int p_vid_in_skip = args_find_int_min(argc, argv, "--vid-in-skip,--fra-skip", def_p_vid_in_skip, 0);
    const int p_vid_in_buff = args_find(argc, argv, "--vid-in-buff,--video-buff");
    const int p_vid_in_loop = args_find_int_min(argc, argv, "--vid-in-loop,--video-loop", def_p_vid_in_loop, 1);
    const int p_vid_in_threads = args_find_int_min(argc, argv, "--vid-in-threads,--ffmpeg-threads", def_p_vid_in_threads, 0);
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
    const char* p_out_probes = args_find_char(argc, argv, "--rt-prb-path,--out-probes", def_p_out_probes);
#ifdef FMDT_ENABLE_PIPELINE
    vec_int_t p_pip_threads = args_find_vector_int(argc, argv, "--pip-threads", def_p_pip_threads);  
    vec_int_t p_pip_sync = args_find_vector_int(argc, argv, "--pip-sync", def_p_pip_sync); 
    vec_int_t p_pip_wait = args_find_vector_int(argc, argv, "--pip-wait", def_p_pip_wait); 
    vec_int_t p_pip_pin = args_find_vector_int(argc, argv, "--pip-pin", def_p_pip_pin);
    vec2D_int_t p_pip_pin_vals = args_find_vector2D_int(argc, argv, "--pip-pin-vals", def_p_pip_pin_vals);

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
    printf("#  * vid-in-path    = %s\n", p_vid_in_path);
    printf("#  * vid-in-start   = %d\n", p_vid_in_start);
    printf("#  * vid-in-stop    = %d\n", p_vid_in_stop);
    printf("#  * vid-in-skip    = %d\n", p_vid_in_skip);
    printf("#  * vid-in-buff    = %d\n", p_vid_in_buff);
    printf("#  * vid-in-loop    = %d\n", p_vid_in_loop);
    printf("#  * vid-in-threads = %d\n", p_vid_in_threads);
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
    printf("#  * rt-prb-path    = %s\n", p_out_probes);
#ifdef FMDT_ENABLE_PIPELINE
    char str_pip_threads[50], str_pip_sync[50], str_pip_wait[50], str_pip_pin[50], str_pip_pin_vals[50];
    args_convert_int_vector_to_string(p_pip_threads, str_pip_threads);
    args_convert_int_vector_to_string(p_pip_sync, str_pip_sync);
    args_convert_int_vector_to_string(p_pip_wait, str_pip_wait);
    args_convert_int_vector_to_string(p_pip_pin, str_pip_pin);
    args_convert_int_vector2D_to_string(p_pip_pin_vals, str_pip_pin_vals);
    printf("#  * pip-threads    = %s\n", str_pip_threads); 
    printf("#  * pip-sync       = %s\n", str_pip_sync); 
    printf("#  * pip-wait       = %s\n", str_pip_wait); 
    printf("#  * pip-pin-enable = %s\n", str_pip_pin);
    printf("#  * pip-pin        = %s\n", str_pip_pin_vals);
#endif
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
#ifdef FMDT_ENABLE_PIPELINE
    if (args_find(argc, argv, "--pip-pin") && !(args_find(argc, argv, "--pip-pin-vals"))) {
        fprintf(stderr, "(WW) '--pip-pin-enable' has to be combined with the '--pip-pin' parameter'\n");
    }
#else
    if (p_out_probes)
        fprintf(stderr, "(WW) Using '--rt-prb-path' without pipeline is not very useful...\n");
#endif
#ifdef FMDT_OPENCV_LINK
    if (p_ccl_fra_id && !p_ccl_fra_path)
        fprintf(stderr, "(WW) '--ccl-fra-id' has to be combined with the '--ccl-fra-path' parameter\n");
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
    size_t i0, i1, j0, j1;
    Video video(p_vid_in_path, p_vid_in_start, p_vid_in_stop, p_vid_in_skip, p_vid_in_buff, p_vid_in_threads, b);
    i0 = video.get_i0();
    i1 = video.get_i1();
    j0 = video.get_j0();
    j1 = video.get_j1();
    video.set_loop_size(p_vid_in_loop);

    Threshold threshold_min(i0, i1, j0, j1, b, p_ccl_hyst_lo);
    Threshold threshold_max(i0, i1, j0, j1, b, p_ccl_hyst_hi);
    threshold_min.set_custom_name("Thr<min>");
    threshold_max.set_custom_name("Thr<max>");
    CCL_LSL lsl(i0, i1, j0, j1, b);
    Features_extractor extractor(i0, i1, j0, j1, b, MAX_ROI_SIZE_BEFORE_SHRINK);
    extractor.set_custom_name("Extractor");
    Features_merger_CCL_HI merger(i0, i1, j0, j1, b, p_mrp_s_min, p_mrp_s_max, MAX_ROI_SIZE_BEFORE_SHRINK,
                                  MAX_ROI_SIZE);
    merger.set_custom_name("Merger");
    Features_magnitude magnitude(i0, i1, j0, j1, b, MAX_ROI_SIZE);
    magnitude.set_custom_name("Magnitude");
    kNN_matcher matcher(p_knn_k, p_knn_d, p_knn_s, MAX_ROI_SIZE);
    Motion motion(MAX_ROI_SIZE);
    motion.set_custom_name("Motion");
    Tracking tracking(p_trk_ext_d, p_trk_angle, p_trk_ddev, p_trk_all, p_trk_star_min, p_trk_meteor_min,
                      p_trk_meteor_max, p_trk_bb_path, p_trk_mag_path, p_trk_ext_o, p_knn_s, MAX_ROI_SIZE);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_id(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_xmin(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_xmax(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_ymin(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_ymax(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_S(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_Sx(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_Sy(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint64_t> delayer_RoIs_Sx2(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint64_t> delayer_RoIs_Sy2(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint64_t> delayer_RoIs_Sxy(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<float> delayer_RoIs_x(MAX_ROI_SIZE, 0.f);
    aff3ct::module::Delayer<float> delayer_RoIs_y(MAX_ROI_SIZE, 0.f);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_magnitude(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_RoIs_sat_count(MAX_ROI_SIZE, 0);
    aff3ct::module::Delayer<uint32_t> delayer_n_RoIs(1, 0);
    delayer_RoIs_id.set_custom_name("D<RoIs_id>");
    delayer_RoIs_xmin.set_custom_name("D<RoIs_xmin>");
    delayer_RoIs_xmax.set_custom_name("D<RoIs_xmax>");
    delayer_RoIs_ymin.set_custom_name("D<RoIs_ymin>");
    delayer_RoIs_ymax.set_custom_name("D<RoIs_ymax>");
    delayer_RoIs_S.set_custom_name("D<RoIs_S>");
    delayer_RoIs_Sx.set_custom_name("D<RoIs_Sx>");
    delayer_RoIs_Sy.set_custom_name("D<RoIs_Sy>");
    delayer_RoIs_Sx2.set_custom_name("D<RoIs_Sx2>");
    delayer_RoIs_Sy2.set_custom_name("D<RoIs_Sy2>");
    delayer_RoIs_Sxy.set_custom_name("D<RoIs_Sxy>");
    delayer_RoIs_x.set_custom_name("D<RoIs_x>");
    delayer_RoIs_y.set_custom_name("D<RoIs_y>");
    delayer_RoIs_magnitude.set_custom_name("D<RoIs_mag>");
    delayer_RoIs_sat_count.set_custom_name("D<RoIs_sat>");
    delayer_n_RoIs.set_custom_name("D<n_RoIs>");
    Logger_RoIs log_RoIs(p_log_path ? p_log_path : "", p_vid_in_start, p_vid_in_skip, MAX_ROI_SIZE, tracking.get_data(),
                         p_trk_mag_path != NULL, p_trk_mag_path != NULL);
    Logger_kNN log_kNN(p_log_path ? p_log_path : "", p_vid_in_start, MAX_ROI_SIZE);
    Logger_motion log_motion(p_log_path ? p_log_path : "", p_vid_in_start);
    log_motion.set_custom_name("Logger_motio");
    Logger_tracks log_track(p_log_path ? p_log_path : "", p_vid_in_start, tracking.get_data());
    log_track.set_custom_name("Logger_trk");
    std::unique_ptr<Logger_frame> log_frame;
    if (p_ccl_fra_path)
        log_frame.reset(new Logger_frame(p_ccl_fra_path, p_vid_in_start, p_ccl_fra_id, i0, i1, j0, j1, b, MAX_ROI_SIZE));

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
        video[vid::tsk::generate] = (*prb_ts_s1b)[aff3ct::module::prb::sck::probe_noin::status];
        (*prb_ts_s1e)[aff3ct::module::prb::tsk::probe] = video[vid::sck::generate::out_img];
        (*ts_s2b)("exec") = video[vid::sck::generate::out_img];
        (*prb_ts_s2b)[aff3ct::module::prb::sck::probe::in] = (*ts_s2b)["exec::out"];
    }

    // step 1: threshold low
    threshold_min[thr::sck::apply::in_img] = video[vid::sck::generate::out_img];

    // step 2: CCL/CCA
    lsl[ccl::sck::apply::in_img] = threshold_min[thr::sck::apply::out_img];
    extractor[ftr_ext::sck::extract::in_img] = lsl[ccl::sck::apply::out_labels];
    extractor[ftr_ext::sck::extract::in_n_RoIs] = lsl[ccl::sck::apply::out_n_RoIs];

    // step 3: hysteresis threshold & surface filtering
    threshold_max[thr::sck::apply::in_img] = video[vid::sck::generate::out_img];
    merger[ftr_mrg::sck::merge::in_labels] = lsl[ccl::sck::apply::out_labels];
    merger[ftr_mrg::sck::merge::in_img_HI] = threshold_max[thr::sck::apply::out_img];
    merger[ftr_mrg::sck::merge::in_RoIs_id] = extractor[ftr_ext::sck::extract::out_RoIs_id];
    merger[ftr_mrg::sck::merge::in_RoIs_xmin] = extractor[ftr_ext::sck::extract::out_RoIs_xmin];
    merger[ftr_mrg::sck::merge::in_RoIs_xmax] = extractor[ftr_ext::sck::extract::out_RoIs_xmax];
    merger[ftr_mrg::sck::merge::in_RoIs_ymin] = extractor[ftr_ext::sck::extract::out_RoIs_ymin];
    merger[ftr_mrg::sck::merge::in_RoIs_ymax] = extractor[ftr_ext::sck::extract::out_RoIs_ymax];
    merger[ftr_mrg::sck::merge::in_RoIs_S] = extractor[ftr_ext::sck::extract::out_RoIs_S];
    merger[ftr_mrg::sck::merge::in_RoIs_Sx] = extractor[ftr_ext::sck::extract::out_RoIs_Sx];
    merger[ftr_mrg::sck::merge::in_RoIs_Sy] = extractor[ftr_ext::sck::extract::out_RoIs_Sy];
    merger[ftr_mrg::sck::merge::in_RoIs_Sx2] = extractor[ftr_ext::sck::extract::out_RoIs_Sx2];
    merger[ftr_mrg::sck::merge::in_RoIs_Sy2] = extractor[ftr_ext::sck::extract::out_RoIs_Sy2];
    merger[ftr_mrg::sck::merge::in_RoIs_Sxy] = extractor[ftr_ext::sck::extract::out_RoIs_Sxy];
    merger[ftr_mrg::sck::merge::in_RoIs_x] = extractor[ftr_ext::sck::extract::out_RoIs_x];
    merger[ftr_mrg::sck::merge::in_RoIs_y] = extractor[ftr_ext::sck::extract::out_RoIs_y];
    merger[ftr_mrg::sck::merge::in_n_RoIs] = lsl[ccl::sck::apply::out_n_RoIs];

    // step 3.5 : compute magnitude for each RoI
    magnitude[ftr_mgn::sck::compute::in_img] = video[vid::sck::generate::out_img];
    magnitude[ftr_mgn::sck::compute::in_labels] = merger[ftr_mrg::sck::merge::out_labels];
    magnitude[ftr_mgn::sck::compute::in_RoIs_xmin] = merger[ftr_mrg::sck::merge::out_RoIs_xmin];
    magnitude[ftr_mgn::sck::compute::in_RoIs_xmax] = merger[ftr_mrg::sck::merge::out_RoIs_xmax];
    magnitude[ftr_mgn::sck::compute::in_RoIs_ymin] = merger[ftr_mrg::sck::merge::out_RoIs_ymin];
    magnitude[ftr_mgn::sck::compute::in_RoIs_ymax] = merger[ftr_mrg::sck::merge::out_RoIs_ymax];
    magnitude[ftr_mgn::sck::compute::in_RoIs_S] = merger[ftr_mrg::sck::merge::out_RoIs_S];
    magnitude[ftr_mgn::sck::compute::in_n_RoIs] = merger[ftr_mrg::sck::merge::out_n_RoIs];

    if (p_out_probes) {
        (*ts_s2e)("exec") = merger[ftr_mrg::sck::merge::out_RoIs_id];
        (*prb_ts_s2e)[aff3ct::module::prb::sck::probe::in] = (*ts_s2e)["exec::out"];
        (*prb_ts_s3b)[aff3ct::module::prb::tsk::probe] = (*prb_ts_s2e)[aff3ct::module::prb::sck::probe::status];
    }

    // step 3.5 : delayer => save t - 1 RoI statistics
    delayer_RoIs_id[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_xmin[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_xmax[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_ymin[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_ymax[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_S[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_Sx[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_Sy[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_Sx2[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_Sy2[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_Sxy[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_x[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_y[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_magnitude[aff3ct::module::dly::tsk::produce] = magnitude[ftr_mgn::sck::compute::out_RoIs_magnitude];
    delayer_RoIs_sat_count[aff3ct::module::dly::tsk::produce] = magnitude[ftr_mgn::sck::compute::out_RoIs_sat_count];
    delayer_n_RoIs[aff3ct::module::dly::tsk::produce] = merger[ftr_mrg::sck::merge::out_RoIs_id];

    // step 4: k-NN matching
    matcher[knn::sck::match::in_RoIs0_id] = delayer_RoIs_id[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_RoIs0_S] = delayer_RoIs_S[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_RoIs0_x] = delayer_RoIs_x[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_RoIs0_y] = delayer_RoIs_y[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_n_RoIs0] = delayer_n_RoIs[aff3ct::module::dly::sck::produce::out];
    matcher[knn::sck::match::in_RoIs1_id] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    matcher[knn::sck::match::in_RoIs1_S] = merger[ftr_mrg::sck::merge::out_RoIs_S];
    matcher[knn::sck::match::in_RoIs1_x] = merger[ftr_mrg::sck::merge::out_RoIs_x];
    matcher[knn::sck::match::in_RoIs1_y] = merger[ftr_mrg::sck::merge::out_RoIs_y];
    matcher[knn::sck::match::in_n_RoIs1] = merger[ftr_mrg::sck::merge::out_n_RoIs];

    // step 5: motion estimation
    motion[mtn::sck::compute::in_RoIs0_x] = delayer_RoIs_x[aff3ct::module::dly::sck::produce::out];
    motion[mtn::sck::compute::in_RoIs0_y] = delayer_RoIs_y[aff3ct::module::dly::sck::produce::out];
    motion[mtn::sck::compute::in_RoIs1_x] = merger[ftr_mrg::sck::merge::out_RoIs_x];
    motion[mtn::sck::compute::in_RoIs1_y] = merger[ftr_mrg::sck::merge::out_RoIs_y];
    motion[mtn::sck::compute::in_RoIs1_prev_id] = matcher[knn::sck::match::out_RoIs1_prev_id];
    motion[mtn::sck::compute::in_n_RoIs1] = merger[ftr_mrg::sck::merge::out_n_RoIs];

    // step 6 : tracking
    tracking[trk::sck::perform::in_frame] = video[vid::sck::generate::out_frame];
    tracking[trk::sck::perform::in_RoIs_id] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    tracking[trk::sck::perform::in_RoIs_xmin] = merger[ftr_mrg::sck::merge::out_RoIs_xmin];
    tracking[trk::sck::perform::in_RoIs_xmax] = merger[ftr_mrg::sck::merge::out_RoIs_xmax];
    tracking[trk::sck::perform::in_RoIs_ymin] = merger[ftr_mrg::sck::merge::out_RoIs_ymin];
    tracking[trk::sck::perform::in_RoIs_ymax] = merger[ftr_mrg::sck::merge::out_RoIs_ymax];
    tracking[trk::sck::perform::in_RoIs_S] = merger[ftr_mrg::sck::merge::out_RoIs_S];
    tracking[trk::sck::perform::in_RoIs_x] = merger[ftr_mrg::sck::merge::out_RoIs_x];
    tracking[trk::sck::perform::in_RoIs_y] = merger[ftr_mrg::sck::merge::out_RoIs_y];
    tracking[trk::sck::perform::in_RoIs_error] = motion[mtn::sck::compute::out_RoIs1_error];
    tracking[trk::sck::perform::in_RoIs_prev_id] = matcher[knn::sck::match::out_RoIs1_prev_id];
    tracking[trk::sck::perform::in_RoIs_magnitude] = magnitude[ftr_mgn::sck::compute::out_RoIs_magnitude];
    tracking[trk::sck::perform::in_n_RoIs] = merger[ftr_mrg::sck::merge::out_n_RoIs];
    tracking[trk::sck::perform::in_motion_est] = motion[mtn::sck::compute::out_motion_est2];

    delayer_RoIs_id[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_id];
    delayer_RoIs_xmin[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_xmin];
    delayer_RoIs_xmax[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_xmax];
    delayer_RoIs_ymin[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_ymin];
    delayer_RoIs_ymax[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_ymax];
    delayer_RoIs_S[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_S];
    delayer_RoIs_Sx[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_Sx];
    delayer_RoIs_Sy[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_Sy];
    delayer_RoIs_Sx2[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_Sx2];
    delayer_RoIs_Sy2[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_Sy2];
    delayer_RoIs_Sxy[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_Sxy];
    delayer_RoIs_x[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_x];
    delayer_RoIs_y[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_RoIs_y];
    delayer_RoIs_magnitude[aff3ct::module::dly::sck::memorize::in] = magnitude[ftr_mgn::sck::compute::out_RoIs_magnitude];
    delayer_RoIs_sat_count[aff3ct::module::dly::sck::memorize::in] = magnitude[ftr_mgn::sck::compute::out_RoIs_sat_count];
    delayer_n_RoIs[aff3ct::module::dly::sck::memorize::in] = merger[ftr_mrg::sck::merge::out_n_RoIs];

    if (p_log_path) {
        log_RoIs[lgr_roi::sck::write::in_RoIs0_id] = delayer_RoIs_id[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_xmin] = delayer_RoIs_xmin[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_xmax] = delayer_RoIs_xmax[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_ymin] = delayer_RoIs_ymin[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_ymax] = delayer_RoIs_ymax[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_S] = delayer_RoIs_S[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_Sx] = delayer_RoIs_Sx[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_Sy] = delayer_RoIs_Sy[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_Sx2] = delayer_RoIs_Sx2[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_Sy2] = delayer_RoIs_Sy2[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_Sxy] = delayer_RoIs_Sxy[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_x] = delayer_RoIs_x[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_y] = delayer_RoIs_y[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_magnitude] = delayer_RoIs_magnitude[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs0_sat_count] = delayer_RoIs_sat_count[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_n_RoIs0] = delayer_n_RoIs[aff3ct::module::dly::sck::produce::out];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_id] = merger[ftr_mrg::sck::merge::out_RoIs_id];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_xmin] = merger[ftr_mrg::sck::merge::out_RoIs_xmin];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_xmax] = merger[ftr_mrg::sck::merge::out_RoIs_xmax];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_ymin] = merger[ftr_mrg::sck::merge::out_RoIs_ymin];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_ymax] = merger[ftr_mrg::sck::merge::out_RoIs_ymax];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_S] = merger[ftr_mrg::sck::merge::out_RoIs_S];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_Sx] = merger[ftr_mrg::sck::merge::out_RoIs_Sx];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_Sy] = merger[ftr_mrg::sck::merge::out_RoIs_Sy];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_Sx2] = merger[ftr_mrg::sck::merge::out_RoIs_Sx2];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_Sy2] = merger[ftr_mrg::sck::merge::out_RoIs_Sy2];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_Sxy] = merger[ftr_mrg::sck::merge::out_RoIs_Sxy];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_x] = merger[ftr_mrg::sck::merge::out_RoIs_x];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_y] = merger[ftr_mrg::sck::merge::out_RoIs_y];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_magnitude] = magnitude[ftr_mgn::sck::compute::out_RoIs_magnitude];
        log_RoIs[lgr_roi::sck::write::in_RoIs1_sat_count] = magnitude[ftr_mgn::sck::compute::out_RoIs_sat_count];
        log_RoIs[lgr_roi::sck::write::in_n_RoIs1] = merger[ftr_mrg::sck::merge::out_n_RoIs];
        log_RoIs[lgr_roi::sck::write::in_frame] = video[vid::sck::generate::out_frame];

        log_kNN[lgr_knn::sck::write::in_data_nearest] = matcher[knn::sck::match::out_data_nearest];
        log_kNN[lgr_knn::sck::write::in_data_distances] = matcher[knn::sck::match::out_data_distances];
        log_kNN[lgr_knn::sck::write::in_data_conflicts] = matcher[knn::sck::match::out_data_conflicts];
        log_kNN[lgr_knn::sck::write::in_RoIs0_id] = delayer_RoIs_id[aff3ct::module::dly::sck::produce::out];
        log_kNN[lgr_knn::sck::write::in_RoIs0_next_id] = matcher[knn::sck::match::out_RoIs0_next_id];
        log_kNN[lgr_knn::sck::write::in_n_RoIs0] = delayer_n_RoIs[aff3ct::module::dly::sck::produce::out];
        log_kNN[lgr_knn::sck::write::in_RoIs1_dx] = motion[mtn::sck::compute::out_RoIs1_dx];
        log_kNN[lgr_knn::sck::write::in_RoIs1_dy] = motion[mtn::sck::compute::out_RoIs1_dy];
        log_kNN[lgr_knn::sck::write::in_RoIs1_error] = motion[mtn::sck::compute::out_RoIs1_error];
        log_kNN[lgr_knn::sck::write::in_RoIs1_is_moving] = motion[mtn::sck::compute::out_RoIs1_is_moving];
        log_kNN[lgr_knn::sck::write::in_n_RoIs1] = merger[ftr_mrg::sck::merge::out_n_RoIs];
        log_kNN[lgr_knn::sck::write::in_frame] = video[vid::sck::generate::out_frame];

        log_motion[lgr_mtn::sck::write::in_motion_est1] = motion[mtn::sck::compute::out_motion_est1];
        log_motion[lgr_mtn::sck::write::in_motion_est2] = motion[mtn::sck::compute::out_motion_est2];
        log_motion[lgr_mtn::sck::write::in_frame] = video[vid::sck::generate::out_frame];

        log_track[lgr_trk::sck::write::in_frame] = video[vid::sck::generate::out_frame];
    }

    if (p_ccl_fra_path) {
        (*log_frame)[lgr_fra::sck::write::in_labels] = merger[ftr_mrg::sck::merge::out_labels];
        (*log_frame)[lgr_fra::sck::write::in_RoIs_id] = merger[ftr_mrg::sck::merge::out_RoIs_id];
        (*log_frame)[lgr_fra::sck::write::in_RoIs_xmax] = merger[ftr_mrg::sck::merge::out_RoIs_xmax];
        (*log_frame)[lgr_fra::sck::write::in_RoIs_ymin] = merger[ftr_mrg::sck::merge::out_RoIs_ymin];
        (*log_frame)[lgr_fra::sck::write::in_RoIs_ymax] = merger[ftr_mrg::sck::merge::out_RoIs_ymax];
        (*log_frame)[lgr_fra::sck::write::in_n_RoIs] = merger[ftr_mrg::sck::merge::out_n_RoIs];
    }

    if (p_out_probes) {
        (*prb_fra_id  )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_thr )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_lat )[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        (*prb_thr_time)[aff3ct::module::prb::tsk::probe] = tracking[trk::sck::perform::status];
        if (p_ccl_fra_path)
            (*prb_ts_s3e)[aff3ct::module::prb::tsk::probe] = (*log_frame)[lgr_fra::sck::write::status];
        else if (p_log_path)
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
        first_task = &video[vid::tsk::generate];

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
            { &video[vid::tsk::generate], },
            { &video[vid::tsk::generate], },
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
            { &delayer_RoIs_id[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_xmin[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_xmax[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_ymin[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_ymax[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_S[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sx[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sy[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sx2[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sy2[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sxy[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_x[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_y[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_magnitude[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_sat_count[aff3ct::module::dly::tsk::produce],
              &delayer_n_RoIs[aff3ct::module::dly::tsk::produce],
              &matcher[knn::tsk::match],
              &motion[mtn::tsk::compute],
              &tracking[trk::tsk::perform],
              &delayer_RoIs_id[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_xmin[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_xmax[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_ymin[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_ymax[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_S[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sx[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sy[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sx2[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sy2[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sxy[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_x[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_y[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_magnitude[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_sat_count[aff3ct::module::dly::tsk::memorize],
              &delayer_n_RoIs[aff3ct::module::dly::tsk::memorize],
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
            { &video[vid::tsk::generate], },
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
              &delayer_RoIs_id[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_xmin[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_xmax[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_ymin[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_ymax[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_S[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sx[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sy[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sx2[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sy2[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_Sxy[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_x[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_y[aff3ct::module::dly::tsk::produce],
              &delayer_RoIs_magnitude[aff3ct::module::dly::tsk::produce],
              &delayer_n_RoIs[aff3ct::module::dly::tsk::produce],
              &matcher[knn::tsk::match],
              &motion[mtn::tsk::compute],
              &tracking[trk::tsk::perform],
              &delayer_RoIs_id[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_xmin[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_xmax[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_ymin[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_ymax[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_S[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sx[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sy[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sx2[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sy2[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_Sxy[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_x[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_y[aff3ct::module::dly::tsk::memorize],
              &delayer_RoIs_magnitude[aff3ct::module::dly::tsk::memorize],
              &delayer_n_RoIs[aff3ct::module::dly::tsk::memorize],
              },
            { },
            { /* no exclusions in this stage */ } ),
        };
    }

    if (p_log_path) {
        std::get<0>(sep_stages[2]).push_back(&log_RoIs[lgr_roi::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_kNN[lgr_knn::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_motion[lgr_mtn::tsk::write]);
        std::get<0>(sep_stages[2]).push_back(&log_track[lgr_trk::tsk::write]);
    }

    if (p_ccl_fra_path) {
        std::get<0>(sep_stages[2]).push_back(&(*log_frame)[lgr_fra::tsk::write]);
    }

    aff3ct::runtime::Pipeline sequence_or_pipeline({ first_task }, // first task of the sequence
                                                sep_stages,
                                                tools_convert_int_cvector_int_stdvector(p_pip_threads), 
                                                tools_convert_int_cvector_int_stdvector(p_pip_sync), 
                                                tools_convert_int_cvector_bool_stdvector(p_pip_wait),
                                                tools_convert_int_cvector_bool_stdvector(p_pip_pin),
                                                tools_convert_int_cvector2D_int_stdvector2D(p_pip_pin_vals));

    
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
        for (size_t s = 0; s < stages.size(); s++) {
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

#ifdef FMDT_ENABLE_PIPELINE
    // ----------
    // -- FREE --
    // ----------

    vector_free(p_pip_threads);
    vector_free(p_pip_sync);
    vector_free(p_pip_wait);
    vector_free(p_pip_pin);
    int size = vector_size(p_pip_pin_vals);
    for(int i = 0; i < size; i++)
        vector_free(p_pip_pin_vals[i]);
    vector_free(p_pip_pin_vals);
    
#endif

    printf("# End of the program, exiting.\n");

#ifdef FMDT_ENABLE_PIPELINE
    sequence_or_pipeline.unbind_adaptors();
#endif

    return EXIT_SUCCESS;
}
