#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <nrc2.h>

#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "fmdt/macros.h"
#include "fmdt/video/video_struct.h"
#include "vec.h"

#include "fmdt/CCL.h"
#include "fmdt/features.h"
#include "fmdt/kNN.h"
#include "fmdt/motion.h"
#include "fmdt/threshold.h"
#include "fmdt/tracking.h"
#include "fmdt/video.h"
#include "fmdt/image.h"
#include "fmdt/version.h"

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
    int def_p_maxred_r = 2;
    char* def_p_ccl_fra_path = NULL;
    int def_p_mrp_s_min = 5;
    int def_p_mrp_s_max = 1000;
    float def_p_ellipse = 5;
    char* def_p_log_path = NULL;
    int def_p_cca_roi_max1 = 65535; // Maximum number of RoIs before `features_merge_CCL_HI` selection.
    int def_p_cca_roi_max2 = 400; // Maximum number of RoIs after `features_merge_CCL_HI` selection.

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
                "  --mxr-r             Radius for the max reduction                                           [%d]\n",
                def_p_maxred_r);
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
                "  --cca-roi-max1      Maximum number of RoIs before hysteresis                               [%d]\n",
                def_p_cca_roi_max1);
        fprintf(stderr,
                "  --cca-roi-max2      Maximum number of RoIs after hysteresis                                [%d]\n",
                def_p_cca_roi_max2);
        fprintf(stderr,
                "  --mrp-s-min         Minimum surface of the CCs in pixels                                   [%d]\n",
                def_p_mrp_s_min);
        fprintf(stderr,
                "  --mrp-s-max         Maxumum surface of the CCs in pixels                                   [%d]\n",
                def_p_mrp_s_max);
        fprintf(stderr,
                "  --eli-r             Minimum value of the ratio for the ellipse filter                      [%f]\n",
                def_p_ellipse);
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
        version_print("ellipse");
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
    const int p_maxred_r = args_find_int_min(argc, argv, "--mxr-r,--red_diam", def_p_maxred_r, 0);    
    const char* p_ccl_fra_path = args_find_char(argc, argv, "--ccl-fra-path,--out-frames", def_p_ccl_fra_path);
#ifdef FMDT_OPENCV_LINK
    const int p_ccl_fra_id = args_find(argc, argv, "--ccl-fra-id,--show-id");
#else
    const int p_ccl_fra_id = 0;
#endif
    const int p_cca_roi_max1 = args_find_int_min(argc, argv, "--cca-roi-max1", def_p_cca_roi_max1, 0);
    const int p_cca_roi_max2 = args_find_int_min(argc, argv, "--cca-roi-max2", def_p_cca_roi_max2, 0);
    const int p_mrp_s_min = args_find_int_min(argc, argv, "--mrp-s-min,--surface-min", def_p_mrp_s_min, 0);
    const int p_mrp_s_max = args_find_int_min(argc, argv, "--mrp-s-max,--surface-max", def_p_mrp_s_max, 0);
    const float p_ellipse = args_find_float_min(argc, argv, "--eli-r", def_p_ellipse, 0);

    const char* p_log_path = args_find_char(argc, argv, "--log-path,--out-stats", def_p_log_path);

    // heading display
    printf("#  ----------------------\n");
    printf("# |           ----*      |\n");
    printf("# | --* FMDT-ELLIPSE --* |\n");
    printf("# |   -------*           |\n");
    printf("#  ----------------------\n");
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
    printf("#  * mxr-r          = %d\n", p_maxred_r);
    printf("#  * ccl-hyst-lo    = %d\n", p_ccl_hyst_lo);
    printf("#  * ccl-hyst-hi    = %d\n", p_ccl_hyst_hi);
    printf("#  * ccl-fra-path   = %s\n", p_ccl_fra_path);
#ifdef FMDT_OPENCV_LINK
    printf("#  * ccl-fra-id     = %d\n", p_ccl_fra_id);
#endif
    printf("#  * cca-roi-max1   = %d\n", p_cca_roi_max1);
    printf("#  * cca-roi-max2   = %d\n", p_cca_roi_max2);
    printf("#  * mrp-s-min      = %d\n", p_mrp_s_min);
    printf("#  * mrp-s-max      = %d\n", p_mrp_s_max);
    printf("#  * eli-r          = %.2f\n", p_ellipse);
    printf("#  * log-path       = %s\n", p_log_path);

    printf("#\n");

    // arguments checking
    if (!p_vid_in_path) {
        fprintf(stderr, "(EE) '--vid-in-path' is missing\n");
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

    int maxred_diam = 2 * p_maxred_r + 1;
    int ellipse_nb = 0;

    // --------------------------------------- //
    // -- VIDEO ALLOCATION & INITIALISATION -- //
    // --------------------------------------- //

    int i0, i1, j0, j1; // image dimension (i0 = y_min, i1 = y_max, j0 = x_min, j1 = x_max)
    video_reader_t* video = video_reader_alloc_init(p_vid_in_path, p_vid_in_start, p_vid_in_stop, p_vid_in_skip,
                                                    p_vid_in_buff, p_vid_in_threads, VCDC_FFMPEG_IO, VCDC_HWACCEL_NONE,
                                                    PIXFMT_GRAY8, 0, NULL, &i0, &i1, &j0, &j1);
    video->loop_size = (size_t)(p_vid_in_loop);
    video_writer_t* video_writer = NULL;
    img_data_t* img_data = NULL;
    if (p_ccl_fra_path) {
        img_data = image_gs_alloc((i1 - i0) + 1, (j1 - j0) + 1);
        const size_t n_threads = 1;
        video_writer = video_writer_alloc_init(p_ccl_fra_path, p_vid_in_start, n_threads, (i1 - i0) + 1, (j1 - j0) + 1,
                                               PIXFMT_GRAY8, VCDC_FFMPEG_IO, 0, 0, NULL);
    }

    // --------------------- //
    // -- DATA ALLOCATION -- //
    // --------------------- //

    RoIs_t* RoIs_tmp = features_alloc_RoIs(p_cca_roi_max1, true, true, false, true);
    RoIs_t* RoIs = features_alloc_RoIs(p_cca_roi_max2, true, true, false, true);
    CCL_data_t* ccl_data = CCL_LSL_alloc_data(i0, i1, j0, j1);

    int b = 1; // image border
    uint8_t **I = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // grayscale input image
    uint8_t **IL = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // binary image (after threshold low) 
    uint8_t ** Max = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // image de max-reduction temporelle 
    uint8_t **IH = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // binary image (after threshold high)
    uint32_t **L1 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // labels (CCL)
    uint32_t **L2 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // labels (CCL + hysteresis)

    // buffer circulaire d'images 
    uint8_t **T[maxred_diam];
    for (int t = 0; t < maxred_diam; t++) {
        T[t] = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);
        zero_ui8matrix(T[t], i0 - b, i1 + b, j0 - b, j1 + b);
    }

    // ------------------------- //
    // -- DATA INITIALISATION -- //
    // ------------------------- //

    features_init_RoIs(RoIs_tmp);
    features_init_RoIs(RoIs);
    CCL_LSL_init_data(ccl_data);
    zero_ui8matrix(I, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(IL, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(L1, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(IH, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(L2, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(Max, i0 - b, i1 + b, j0 - b, j1 + b);

    // ----------------//
    // -- PROCESSING --//
    // ----------------//

    printf("# The program is running...\n");
    int cur_fra;
    while ((cur_fra = video_reader_get_frame(video, I, NULL)) != -1) {
        fprintf(stderr, "(II) Frame n°%4d", cur_fra);

        // step 1: max-reduction
        tools_copy_ui8matrix_ui8matrix((const uint8_t**)I, i0 - b, i1 + b, j0 - b, j1 + b, T[cur_fra % maxred_diam]);
        zero_ui8matrix(Max, i0 - b, i1 + b, j0 - b, j1 + b);
        for (int k = 0; k < maxred_diam; k++) {
            // max temporel
            image_max_reduce(T[k], i0, i1, j0, j1, Max);
        }

        // step 2: threshold low
        threshold((const uint8_t**)Max, IL, i0, i1, j0, j1, p_ccl_hyst_lo);

        // step 3: CCL/CCA
        RoIs_tmp->_size = CCL_LSL_apply(ccl_data, (const uint8_t**)IL, L1, 0);
        assert(RoIs_tmp->_size <= RoIs_tmp->_max_size);
        features_extract((const uint32_t**)L1, i0, i1, j0, j1, RoIs_tmp->basic, RoIs_tmp->_size);

        // step 4: hysteresis threshold & surface filtering
        threshold((const uint8_t**)Max, IH, i0, i1, j0, j1, p_ccl_hyst_hi);
        RoIs->_size = features_merge_CCL_HI_v2((const uint32_t**)L1, (const uint8_t**)IH, L2, i0, i1, j0, j1,
                                               RoIs_tmp->basic, RoIs_tmp->_size, p_mrp_s_min, p_mrp_s_max);
        assert(RoIs->_size <= RoIs->_max_size);
        features_shrink(RoIs_tmp->basic, NULL, NULL, RoIs_tmp->_size, RoIs->basic, NULL, NULL);

        // step 5: ellipse feature computation
        features_compute_ellipse(RoIs->basic, RoIs->elli, RoIs->_size);

        // save stats (first part)
        FILE* f = NULL;
        if (p_log_path) {
            tools_create_folder(p_log_path);
            char filename[1024];
            snprintf(filename, sizeof(filename), "%s/%05d.txt", p_log_path, cur_fra);
            f = fopen(filename, "w");
            if (f == NULL) {
                fprintf(stderr, "(EE) error while opening '%s'\n", filename);
                exit(1);
            }
            fprintf(f, "# Frame n°%05d (BEFORE ellipse ratio threshold) -- ", cur_fra);
            features_RoIs_write(f, cur_fra, RoIs->basic, NULL, RoIs->elli, RoIs->_size , NULL, 0);
            fprintf(f, "#\n");
        }

        // save frames (CCs)
        if (img_data) {
            image_gs_draw_labels(img_data, (const uint32_t**)L2, RoIs->basic, RoIs->_size, p_ccl_fra_id);
            video_writer_save_frame(video_writer, (const uint8_t**)image_gs_get_pixels_2d(img_data));
        }

        // step 6: filter on ellipse ratio
        uint32_t n_RoIs_new = threshold_ellipse_ratio(RoIs->basic, RoIs->elli, RoIs->_size, p_ellipse);
        features_shrink(RoIs->basic, NULL, RoIs->elli, RoIs->_size, RoIs->basic, NULL, RoIs->elli);
        RoIs->_size = n_RoIs_new;

        // save stats (second part)
        if (p_log_path) {
            fprintf(f, "# Frame n°%05d (AFTER ellipse ratio threshold) -- ", cur_fra);
            features_RoIs_write(f, cur_fra, RoIs->basic, NULL, RoIs->elli, RoIs->_size, NULL, 0);
            fclose(f);
        }

        ellipse_nb += RoIs->_size;
        fprintf(stderr, " -- Ellipses =  %3d\r", ellipse_nb);
        fflush(stderr);
    }
    fprintf(stderr, "\n");

    // ----------
    // -- FREE --
    // ----------

    for (int t = 0; t < maxred_diam; t++) {
        free_ui8matrix(T[t], i0 - b, i1 + b, j0 - b, j1 + b);
    }
    free_ui8matrix(Max, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(I, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(IL, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(L1, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(IH, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(L2, i0 - b, i1 + b, j0 - b, j1 + b);
    features_free_RoIs(RoIs_tmp);
    features_free_RoIs(RoIs);
    video_reader_free(video);
    if (img_data) {
        image_gs_free(img_data);
        video_writer_free(video_writer);
    }
    CCL_LSL_free_data(ccl_data);
    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
