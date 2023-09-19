/*
./bin/fmdt-detect-motion --vid-in-path ~/Videos/car3/car_3%03d.pgm --ccl-fra-path cars/%03d.png --mrp-s-min 50 --mrp-s-max 10000 --log-path ./detect_log_car --trk-roi-path ./tracks_2_rois_car.txt --knn-d 10 --trk-angle 180 --trk-meteor-min 4 --trk-all
./bin/fmdt-log-parser --log-path ./detect_log_car --trk-roi-path ./tracks_2_rois_car.txt --trk-path ./out_detect_tracks_car.txt --trk-bb-path ./out_detect_bb_car.txt
./bin/fmdt-visu --vid-in-path ~/Videos/car3/car_3%03d.pgm --trk-path ./out_detect_tracks_car.txt --trk-bb-path ./out_detect_bb_car.txt --vid-out-path visu_car/%04d.png
*/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <nrc2.h>

#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "fmdt/macros.h"

#include "fmdt/CCL.h"
#include "fmdt/features.h"
#include "fmdt/kNN.h"
#include "fmdt/motion.h"
#include "fmdt/threshold.h"
#include "fmdt/tracking.h"
#include "fmdt/video.h"
#include "fmdt/image.h"
#include "fmdt/version.h"
#include "fmdt/video/video_struct.h"

#include "fmdt/sigma_delta.h"
#include "fmdt/morpho.h"

int main(int argc, char** argv) {
    // default values
    char* def_p_vid_in_path = NULL;
    int def_p_vid_in_start = 0;
    int def_p_vid_in_stop = 0;
    int def_p_vid_in_skip = 0;
    int def_p_vid_in_loop = 1;
    int def_p_vid_in_threads = 0;
    char def_p_vid_in_dec[16] = "FFMPEG-IO";
    char def_p_vid_in_dec_hw[16] = "NONE";
    char def_p_ccl_impl[16] = "LSLH";
    char* def_p_ccl_fra_path = NULL;
    int def_p_mrp_s_min = 3;
    int def_p_mrp_s_max = 1000;
    int def_p_knn_k = 3;
    int def_p_knn_d = 10;
    float def_p_knn_s = 0.125f;
    int def_p_trk_ext_d = 5;
    int def_p_trk_ext_o = 3;
    float def_p_trk_angle = 20;
    int def_p_trk_star_min = 15;
    int def_p_trk_meteor_min = 3;
    int def_p_trk_meteor_max = 100;
    char* def_p_trk_roi_path = NULL;
    char* def_p_log_path = NULL;
    int def_p_cca_roi_max = 65535; // Maximum number of RoIs

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
                "  --vid-in-dec        Select video decoder implementation ('FFMPEG-IO' or 'VCODEC-IO')       [%s]\n",
                def_p_vid_in_dec);
        fprintf(stderr,
                "--vid-in-dec-hw       Select video decoder hardware acceleration ('NONE', 'NVDEC', 'VIDTB')  [%s]\n",
                def_p_vid_in_dec_hw);
        fprintf(stderr,
                "  --ccl-impl          Select the CCL implementation to use ('LSLH' or 'LSLM')                [%s]\n",
                def_p_ccl_impl);
        fprintf(stderr,
                "  --ccl-fra-path      Path of the files for CC debug frames                                  [%s]\n",
                def_p_ccl_fra_path ? def_p_ccl_fra_path : "NULL");
#ifdef FMDT_OPENCV_LINK
        fprintf(stderr,
                "  --ccl-fra-id        Show the RoI/CC ids on the ouptut CC frames                                \n");
#endif
        fprintf(stderr,
                "  --cca-roi-max       Maximum number of RoIs                                                 [%d]\n",
                def_p_cca_roi_max);
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
                "  --trk-all           Tracks all object types (star, meteor or noise)                            \n");
        fprintf(stderr,
                "  --trk-roi-path      Path to the file containing the RoI ids for each track                 [%s]\n",
                def_p_trk_roi_path ? def_p_trk_roi_path : "NULL");
        fprintf(stderr,
                "  --log-path          Path of the output statistics, only required for debugging purpose     [%s]\n",
                def_p_log_path ? def_p_log_path : "NULL");
        fprintf(stderr,
                "  --help, -h          This help                                                                  \n");
        fprintf(stderr,
                "  --version, -v       Print the version                                                          \n");
        exit(1);
    }

    // version
    if (args_find(argc, argv, "--version,-v")) {
        version_print("detect");
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
    const char* p_vid_in_dec = args_find_char(argc, argv, "--vid-in-dec", def_p_vid_in_dec);
    const char* p_vid_in_dec_hw = args_find_char(argc, argv, "--vid-in-dec-hw", def_p_vid_in_dec_hw);
    const char* p_ccl_impl = args_find_char(argc, argv, "--ccl-impl", def_p_ccl_impl);
    const char* p_ccl_fra_path = args_find_char(argc, argv, "--ccl-fra-path,--out-frames", def_p_ccl_fra_path);
#ifdef FMDT_OPENCV_LINK
    const int p_ccl_fra_id = args_find(argc, argv, "--ccl-fra-id,--show-id");
#else
    const int p_ccl_fra_id = 0;
#endif
    const int p_cca_roi_max = args_find_int_min(argc, argv, "--cca-roi-max", def_p_cca_roi_max, 0);
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
    const int p_trk_all = args_find(argc, argv, "--trk-all,--track-all");
    const char* p_trk_roi_path = args_find_char(argc, argv, "--trk-roi-path", def_p_trk_roi_path);
    const char* p_log_path = args_find_char(argc, argv, "--log-path,--out-stats", def_p_log_path);

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
    printf("#  * vid-in-dec     = %s\n", p_vid_in_dec);
    printf("#  * vid-in-dec-hw  = %s\n", p_vid_in_dec_hw);
    printf("#  * ccl-impl       = %s\n", p_ccl_impl);
    printf("#  * ccl-fra-path   = %s\n", p_ccl_fra_path);
#ifdef FMDT_OPENCV_LINK
    printf("#  * ccl-fra-id     = %d\n", p_ccl_fra_id);
#endif
    printf("#  * cca-roi-max    = %d\n", p_cca_roi_max);
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
    printf("#  * trk-all        = %d\n", p_trk_all);
    printf("#  * trk-roi-path   = %s\n", p_trk_roi_path);
    printf("#  * log-path       = %s\n", p_log_path);

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
#ifdef FMDT_OPENCV_LINK
    if (p_ccl_fra_id && !p_ccl_fra_path)
        fprintf(stderr, "(WW) '--ccl-fra-id' has to be combined with the '--ccl-fra-path' parameter\n");
#endif

    // --------------------------------------- //
    // -- VIDEO ALLOCATION & INITIALISATION -- //
    // --------------------------------------- //

    TIME_POINT(start_alloc_init);
    int i0, i1, j0, j1; // image dimension (i0 = y_min, i1 = y_max, j0 = x_min, j1 = x_max)
    video_reader_t* video = video_reader_alloc_init(p_vid_in_path, p_vid_in_start, p_vid_in_stop, p_vid_in_skip,
                                                    p_vid_in_buff, p_vid_in_threads, video_str_to_enum(p_vid_in_dec),
                                                    video_hwaccel_str_to_enum(p_vid_in_dec_hw), &i0, &i1, &j0, &j1);
    video->loop_size = (size_t)(p_vid_in_loop);
    video_writer_t* video_writer = NULL;
    img_data_t* img_data = NULL;
    if (p_ccl_fra_path) {
        img_data = image_gs_alloc((j1 - j0) + 1, (i1 - i0) + 1);
        const size_t n_threads = 1;
        video_writer = video_writer_alloc_init(p_ccl_fra_path, p_vid_in_start, n_threads, (i1 - i0) + 1, (j1 - j0) + 1,
                                               PIXFMT_GRAY, VCDC_FFMPEG_IO);
    }

    // --------------------- //
    // -- DATA ALLOCATION -- //
    // --------------------- //

    RoIs_t* RoIs_tmp = features_alloc_RoIs(0, 0, 0, p_cca_roi_max);
    RoIs_t* RoIs0 = features_alloc_RoIs(0, 0, 0, p_cca_roi_max);
    RoIs_t* RoIs1 = features_alloc_RoIs(0, 0, 0, p_cca_roi_max);

    features_free_RoIs_motion(RoIs_tmp->motion, 0); RoIs_tmp->motion = NULL;
    features_free_RoIs_motion(RoIs0->motion, 0); RoIs0->motion = NULL;
    features_free_RoIs_motion(RoIs1->motion, 0); RoIs1->motion = NULL;

    features_free_RoIs_misc(RoIs_tmp->misc, 0); RoIs_tmp->misc = NULL;
    features_free_RoIs_misc(RoIs0->misc, 0); RoIs0->misc = NULL;
    features_free_RoIs_misc(RoIs1->misc, 0); RoIs1->misc = NULL;

    CCL_gen_data_t* ccl_data = CCL_alloc_data(CCL_str_to_enum(p_ccl_impl), i0, i1, j0, j1);
    kNN_data_t* knn_data = kNN_alloc_data(p_cca_roi_max);
    tracking_data_t* tracking_data = tracking_alloc_data(MAX(p_trk_star_min, p_trk_meteor_min), p_cca_roi_max);
    int b = 1; // image border
    uint8_t **I = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // grayscale input image
    uint8_t **IB = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // binary image (after Sigma-Delta)
    uint32_t **L1 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // labels (CCL)
    uint32_t **L2 = NULL; // labels (CCL + surface filter)
    if (p_ccl_fra_path)
        L2 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b);

    sigma_delta_data_t* sd_data = sigma_delta_alloc_data(i0 - b, i1 + b, j0 - b, j1 + b, 1, 254);
    morpho_data_t* morpho_data = morpho_alloc_data(i0 - b, i1 + b, j0 - b, j1 + b);

    // ------------------------- //
    // -- DATA INITIALISATION -- //
    // ------------------------- //

    tracking_init_global_data();
    features_init_RoIs(RoIs_tmp);
    features_init_RoIs(RoIs0);
    features_init_RoIs(RoIs1);
    tracking_init_data(tracking_data);
    CCL_init_data(ccl_data);
    kNN_init_data(knn_data);
    zero_ui8matrix(I, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(IB, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(L1, i0 - b, i1 + b, j0 - b, j1 + b);
    if (p_ccl_fra_path)
        zero_ui32matrix(L2, i0 - b, i1 + b, j0 - b, j1 + b);

    morpho_init_data(morpho_data);

    TIME_POINT(stop_alloc_init);
    printf("# Allocations and initialisations took %6.3f sec\n", TIME_ELAPSED_SEC(start_alloc_init, stop_alloc_init));

    // ---------------- //
    // -- PROCESSING -- //
    // ---------------- //

    printf("# The program is running...\n");
    size_t real_n_tracks = 0;
    unsigned n_frames = 0, n_stars = 0, n_meteors = 0, n_noise = 0;
    int cur_fra;
    TIME_POINT(start_compute);
    while ((cur_fra = video_reader_get_frame(video, I)) != -1) {
        fprintf(stderr, "(II) Frame n°%4d", cur_fra);

        if (n_frames == 0)
            sigma_delta_init_data(sd_data, (const uint8_t**)I, i0, i1, j0, j1);

        // step 1: motion detection with Sigma-Delta algorithm
        const uint8_t N = 2;
        sigma_delta_compute(sd_data, (const uint8_t**)I, IB, i0, i1, j0, j1, N);
        morpho_compute_opening3(morpho_data, (const uint8_t**)IB, IB, i0, i1, j0, j1);
        morpho_compute_closing3(morpho_data, (const uint8_t**)IB, IB, i0, i1, j0, j1);

        // step 2: CCL/CCA
        const uint32_t n_RoIs = CCL_apply(ccl_data, (const uint8_t**)IB, L1, 0);
        assert(n_RoIs <= RoIs_tmp->_max_size);
        features_extract((const uint32_t**)L1, i0, i1, j0, j1, n_RoIs, RoIs_tmp->basic);

        // step 3: surface filtering
        features_filter_surface((const uint32_t**)L1, L2, i0, i1, j0, j1, RoIs_tmp->basic, p_mrp_s_min,
                                p_mrp_s_max);
        features_shrink_basic(RoIs_tmp->basic, RoIs1->basic);

        // step 4: k-NN matching
        kNN_match(knn_data, RoIs0->basic, RoIs1->basic, RoIs0->asso, RoIs1->asso, p_knn_k, p_knn_d, p_knn_s);

        // step 5: tracking
        tracking_perform(tracking_data, RoIs1, cur_fra, NULL, p_trk_ext_d, p_trk_angle, 0.f, p_trk_all,
                         p_trk_star_min, p_trk_meteor_min, p_trk_meteor_max, p_trk_roi_path != NULL, p_trk_ext_o,
                         p_knn_s, 0);

        // save frames (CCs)
        if (img_data) {
            image_gs_draw_labels(img_data, (const uint32_t**)L2, RoIs1->basic, p_ccl_fra_id);
            video_writer_save_frame(video_writer, (const uint8_t**)image_gs_get_pixels_2d(img_data));
        }

        // save stats
        if (p_log_path) {
            tools_create_folder(p_log_path);
            char filename[1024];
            snprintf(filename, sizeof(filename), "%s/%05d.txt", p_log_path, cur_fra);
            FILE* f = fopen(filename, "w");
            if (f == NULL) {
                fprintf(stderr, "(EE) error while opening '%s'\n", filename);
                exit(1);
            }
            int prev_fra = cur_fra > p_vid_in_start ? cur_fra - (p_vid_in_skip + 1) : -1;
            features_RoIs0_RoIs1_write(f, prev_fra, cur_fra, RoIs0->basic, RoIs0->misc, RoIs1->basic, RoIs1->misc,
                                       tracking_data->tracks);
            if (cur_fra > p_vid_in_start) {
                fprintf(f, "#\n");
                kNN_asso_conflicts_write(f, knn_data, RoIs0->asso, RoIs1->asso, RoIs1->motion);
                fprintf(f, "#\n");
                tracking_tracks_write_full(f, tracking_data->tracks);
            }
            fclose(f);
        }

        // swap RoIs0 <-> RoIs1 for the next frame
        RoIs_t* tmp = RoIs0;
        RoIs0 = RoIs1;
        RoIs1 = tmp;

        n_frames++;
        real_n_tracks = tracking_count_objects(tracking_data->tracks, &n_stars, &n_meteors, &n_noise);

        TIME_POINT(stop_compute);
        fprintf(stderr, " -- Time = %6.3f sec", TIME_ELAPSED_SEC(start_compute, stop_compute));
        fprintf(stderr, " -- FPS = %4d", (int)(n_frames / (TIME_ELAPSED_SEC(start_compute, stop_compute))));
        fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\r", n_meteors, n_stars,
                n_noise, (unsigned long)real_n_tracks);
        fflush(stderr);
    }
    TIME_POINT(stop_compute);
    fprintf(stderr, "\n");

    if (p_trk_roi_path) {
        FILE* f = fopen(p_trk_roi_path, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_trk_roi_path);
            exit(1);
        }
        tracking_tracks_RoIs_id_write(f, tracking_data->tracks);
        fclose(f);
    }
    tracking_tracks_write(stdout, tracking_data->tracks);

    printf("# Tracks statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, (unsigned long)real_n_tracks);
    printf("# -> Took %6.3f seconds (avg %d FPS)\n", TIME_ELAPSED_SEC(start_compute, stop_compute),
           (int)(n_frames / (TIME_ELAPSED_SEC(start_compute, stop_compute))));

    // ---------- //
    // -- FREE -- //
    // ---------- //

    free_ui8matrix(I, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(IB, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(L1, i0 - b, i1 + b, j0 - b, j1 + b);
    if (p_ccl_fra_path)
        free_ui32matrix(L2, i0 - b, i1 + b, j0 - b, j1 + b);
    features_free_RoIs(RoIs_tmp);
    features_free_RoIs(RoIs0);
    features_free_RoIs(RoIs1);
    video_reader_free(video);
    if (img_data) {
        image_gs_free(img_data);
        video_writer_free(video_writer);
    }
    CCL_free_data(ccl_data);
    kNN_free_data(knn_data);
    tracking_free_data(tracking_data);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}