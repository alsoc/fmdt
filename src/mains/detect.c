#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <nrc2.h>

#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/CCL.h"
#include "fmdt/features.h"
#include "fmdt/kNN.h"
#include "fmdt/motion.h"
#include "fmdt/threshold.h"
#include "fmdt/tracking.h"
#include "fmdt/video.h"
#include "fmdt/image.h"

int main(int argc, char** argv) {
    // default values
    char* def_p_vid_in_path = NULL;
    int def_p_vid_in_start = 0;
    int def_p_vid_in_stop = 0;
    int def_p_vid_in_skip = 0;
    int def_p_vid_in_loop = 1;
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
                "  --vid-in-buff       Bufferize all the video in global memory before executing the chain        \n");
        fprintf(stderr,
                "  --vid-in-loop       Number of times the video is read in loop                              [%d]\n",
                def_p_vid_in_loop);
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
                "  --help, -h          This help                                                                  \n");
        exit(1);
    }

    // parse arguments
    const char* p_vid_in_path = args_find_char(argc, argv, "--vid-in-path,--in-video", def_p_vid_in_path);
    const int p_vid_in_start = args_find_int_min(argc, argv, "--vid-in-start,--fra-start", def_p_vid_in_start, 0);
    const int p_vid_in_stop = args_find_int_min(argc, argv, "--vid-in-stop,--fra-end", def_p_vid_in_stop, 0);
    const int p_vid_in_skip = args_find_int_min(argc, argv, "--vid-in-skip,--fra-skip", def_p_vid_in_skip, 0);
    const int p_vid_in_buff = args_find(argc, argv, "--vid-in-buff,--video-buff");
    const int p_vid_in_loop = args_find_int_min(argc, argv, "--vid-in-loop,--video-loop", def_p_vid_in_loop, 1);
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

    // --------------------------------------- //
    // -- VIDEO ALLOCATION & INITIALISATION -- //
    // --------------------------------------- //

    int i0, i1, j0, j1; // image dimension (i0 = y_min, i1 = y_max, j0 = x_min, j1 = x_max)
    video_reader_t* video = video_reader_alloc_init(p_vid_in_path, p_vid_in_start, p_vid_in_stop, p_vid_in_skip,
                                                    p_vid_in_buff, p_vid_in_threads, &i0, &i1, &j0, &j1);
    video->loop_size = (size_t)(p_vid_in_loop);
    video_writer_t* video_writer = NULL;
    img_data_t* img_data = NULL;
    if (p_ccl_fra_path) {
        img_data = image_gs_alloc((j1 - j0) + 1, (i1 - i0) + 1);
        const size_t n_threads = 1;
        video_writer = video_writer_alloc_init(p_ccl_fra_path, p_vid_in_start, n_threads, (i1 - i0) + 1, (j1 - j0) + 1,
                                               PIXFMT_GRAY);
    }

    // --------------------- //
    // -- DATA ALLOCATION -- //
    // --------------------- //

    RoIs_t* RoIs_tmp = features_alloc_RoIs(MAX_ROI_SIZE_BEFORE_SHRINK);
    RoIs_t* RoIs0 = features_alloc_RoIs(MAX_ROI_SIZE);
    RoIs_t* RoIs1 = features_alloc_RoIs(MAX_ROI_SIZE);
    CCL_gen_data_t* ccl_data = CCL_alloc_data(CCL_str_to_enum(p_ccl_impl), i0, i1, j0, j1);
    kNN_data_t* knn_data = kNN_alloc_data(MAX_ROI_SIZE);
    vec_BB_t* BBs = NULL;
    if (p_trk_bb_path)
        BBs = (vec_BB_t*)vector_create();
    tracking_data_t* tracking_data = tracking_alloc_data(MAX(p_trk_star_min, p_trk_meteor_min), MAX_ROI_SIZE);
    int b = 1; // image border
    uint8_t **I = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // grayscale input image
    uint8_t **IL = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // binary image (after threshold low)
    uint8_t **IH = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // binary image (after threshold high)
    uint32_t **L1 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // labels (CCL)
    uint32_t **L2 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // labels (CCL + hysteresis)

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
    zero_ui8matrix(IL, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(L1, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(IH, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(L2, i0 - b, i1 + b, j0 - b, j1 + b);

    // ----------------//
    // -- PROCESSING --//
    // ----------------//

    printf("# The program is running...\n");
    size_t real_n_tracks = 0;
    unsigned n_frames = 0, n_stars = 0, n_meteors = 0, n_noise = 0;
    int cur_fra;
    while ((cur_fra = video_reader_get_frame(video, I)) != -1) {
        fprintf(stderr, "(II) Frame n°%4d", cur_fra);

        // step 1: threshold low
        threshold((const uint8_t**)I, IL, i0, i1, j0, j1, p_ccl_hyst_lo);

        // step 2: CCL/CCA
        const int n_RoI = CCL_apply(ccl_data, (const uint8_t**)IL, L1);
        features_extract((const uint32_t**)L1, i0, i1, j0, j1, n_RoI, RoIs_tmp->basic);

        // step 3: hysteresis threshold & surface filtering (+ magnitude computations)
        threshold((const uint8_t**)I, IH, i0, i1, j0, j1, p_ccl_hyst_hi);
        features_merge_CCL_HI_v2((const uint32_t**)L1, (const uint8_t**)IH, L2, i0, i1, j0, j1, RoIs_tmp->basic,
                                 p_mrp_s_min, p_mrp_s_max);
        features_shrink(RoIs_tmp->basic, RoIs1->basic);
        features_compute_magnitude((const uint8_t**)I, j1, i1, (const uint32_t**)L2, RoIs1->basic,
                                   RoIs1->misc);

        // step 4: k-NN matching
        kNN_match(knn_data, RoIs0->basic, RoIs1->basic, RoIs0->asso, RoIs1->asso, p_knn_k, p_knn_d, p_knn_s);

        // step 5: motion estimation
        motion_t motion_est1, motion_est2;
        motion_compute(RoIs0->basic, RoIs1->basic, RoIs1->asso, RoIs1->motion, &motion_est1, &motion_est2);

        // step 6: tracking
        tracking_perform(tracking_data, RoIs1, &BBs, cur_fra, &motion_est2, p_trk_ext_d, p_trk_angle,
                         p_trk_ddev, p_trk_all, p_trk_star_min, p_trk_meteor_min, p_trk_meteor_max,
                         p_trk_mag_path != NULL, p_trk_ext_o, p_knn_s);

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
            if (f) {
                int prev_fra = cur_fra > p_vid_in_start ? cur_fra - (p_vid_in_skip + 1) : -1;
                features_RoIs0_RoIs1_write(f, prev_fra, cur_fra, RoIs0->basic, RoIs0->misc, RoIs1->basic, RoIs1->misc,
                                           tracking_data->tracks);
                if (cur_fra > p_vid_in_start) {
                    fprintf(f, "#\n");
                    kNN_asso_conflicts_write(f, knn_data, RoIs0->asso, RoIs1->asso, RoIs1->motion);
                    fprintf(f, "#\n");
                    motion_write(f, &motion_est1, &motion_est2);
                    fprintf(f, "#\n");
                    tracking_tracks_write_full(f, tracking_data->tracks);
                }
                fclose(f);
            } else {
                fprintf(stderr, "(WW) cannot open '%s' file.", filename);
            }
        }

        n_frames++;
        real_n_tracks = tracking_count_objects(tracking_data->tracks, &n_stars, &n_meteors, &n_noise);
        fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\r", n_meteors, n_stars,
                n_noise, (unsigned long)real_n_tracks);
        fflush(stderr);

        RoIs_t* tmp = RoIs0;
        RoIs0 = RoIs1;
        RoIs1 = tmp;
    }
    fprintf(stderr, "\n");

    if (BBs) {
        FILE* f = fopen(p_trk_bb_path, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_trk_bb_path);
            exit(1);
        }
        tracking_BBs_write(f, BBs, tracking_data->tracks);
        fclose(f);
    }

    if (p_trk_mag_path) {
        FILE* f = fopen(p_trk_mag_path, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_trk_mag_path);
            exit(1);
        }
        tracking_tracks_magnitudes_write(f, tracking_data->tracks);
        fclose(f);
    }
    tracking_tracks_write(stdout, tracking_data->tracks);

    printf("# Tracks statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, (unsigned long)real_n_tracks);

    // ----------
    // -- FREE --
    // ----------

    free_ui8matrix(I, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(IL, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(L1, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(IH, i0 - b, i1 + b, j0 - b, j1 + b);
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
    if (BBs) {
        size_t vs = vector_size(BBs);
        for (size_t i = 0; i < vs; i++)
            vector_free(BBs[i]);
        vector_free(BBs);
    }
    tracking_free_data(tracking_data);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
