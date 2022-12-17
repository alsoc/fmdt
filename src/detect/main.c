#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <nrc2.h>

#include "fmdt/args.h"
#include "fmdt/CCL.h"
#include "fmdt/tools.h"
#include "fmdt/features.h"
#include "fmdt/KNN.h"
#include "fmdt/threshold.h"
#include "fmdt/tracking.h"
#include "fmdt/video.h"
#include "fmdt/images.h"
#include "fmdt/macros.h"
#include "vec.h"

int get_next_frame(video_t* video, images_t* images, uint8_t** I) {
    if (video)
        return video_get_next_frame(video, I);
    else if (images)
        return images_get_next_frame(images, I);
    return 0;
}

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
    char* def_p_out_mag = NULL;
    int def_p_video_loop = 1;
    int def_p_ffmpeg_threads = 0;

    // help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr,
                "  --in-video          Path to video file or to a folder of PGM images                        [%s]\n",
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
                "  -k                  Number of neighbours                                                   [%d]\n",
                def_p_k);
        fprintf(stderr,
                "  --max-dist          Maximum number of pixels between two images (in k-NN)                  [%d]\n",
                def_p_max_dist);
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
                "  --video-buff        Bufferize all the video in global memory before executing the chain        \n");
        fprintf(stderr,
                "  --video-loop        Number of times the video is read in loop                              [%d]\n",
                def_p_video_loop);
        fprintf(stderr,
                "  --ffmpeg-threads    Select the number of threads to use to decode video input (in ffmpeg)  [%d]\n",
                def_p_ffmpeg_threads);
        fprintf(stderr,
                "  -h                  This help                                                                  \n");
        exit(1);
    }

    // parse arguments
    const int p_fra_start = args_find_int(argc, argv, "--fra-start", def_p_fra_start);
    const int p_fra_end = args_find_int(argc, argv, "--fra-end", def_p_fra_end);
    const int p_fra_skip = args_find_int(argc, argv, "--fra-skip", def_p_fra_skip);
    const int p_light_min = args_find_int(argc, argv, "--light-min", def_p_light_min);
    const int p_light_max = args_find_int(argc, argv, "--light-max", def_p_light_max);
    const int p_surface_min = args_find_int(argc, argv, "--surface-min", def_p_surface_min);
    const int p_surface_max = args_find_int(argc, argv, "--surface-max", def_p_surface_max);
    const int p_k = args_find_int(argc, argv, "-k", def_p_k);
    const int p_max_dist = args_find_int(argc, argv, "--max-dist", def_p_max_dist);
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
    const char* p_out_mag = args_find_char(argc, argv, "--out-mag", def_p_out_mag);
    const int p_track_all = args_find(argc, argv, "--track-all");
    const int p_video_buff = args_find(argc, argv, "--video-buff");
    const int p_video_loop = args_find_int(argc, argv, "--video-loop", def_p_video_loop);
    const int p_ffmpeg_threads = args_find_int(argc, argv, "--ffmpeg-threads", def_p_ffmpeg_threads);

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
    printf("#  * r-extrapol     = %d\n", p_r_extrapol);
    printf("#  * angle-max      = %f\n", p_angle_max);
    printf("#  * fra-star-min   = %d\n", p_fra_star_min);
    printf("#  * fra-meteor-min = %d\n", p_fra_meteor_min);
    printf("#  * fra-meteor-max = %d\n", p_fra_meteor_max);
    printf("#  * diff-dev       = %4.2f\n", p_diff_dev);
    printf("#  * track-all      = %d\n", p_track_all);
    printf("#  * video-buff     = %d\n", p_video_buff);
    printf("#  * video-loop     = %d\n", p_video_loop);
    printf("#  * ffmpeg-threads = %d\n", p_ffmpeg_threads);
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
    if (p_fra_end && p_fra_end < p_fra_start) {
        fprintf(stderr, "(EE) '--fra-end' has to be higher than '--fra-start'\n");
        exit(1);
    }
    if (!tools_is_dir(p_in_video) && p_video_buff)
        fprintf(stderr, "(WW) '--video-buff' has no effect when '--in-video' is a video file\n");
    if (!tools_is_dir(p_in_video) && p_video_loop > 1)
        fprintf(stderr, "(WW) '--video-loop' has no effect when '--in-video' is a video file\n");
    if (p_video_loop <= 0) {
        fprintf(stderr, "(EE) '--video-loop' has to be bigger than 0\n");
        exit(1);
    }
    if (p_ffmpeg_threads < 0) {
        fprintf(stderr, "(EE) '--ffmpeg-threads' has to be bigger or equal to 0\n");
        exit(1);
    }
    if (p_ffmpeg_threads && tools_is_dir(p_in_video))
        fprintf(stderr, "(WW) '--ffmpeg-threads' has no effect when '--in-video' is a folder of images\n");

    // -------------------------- //
    // -- VIDEO INITIALISATION -- //
    // -------------------------- //

    int i0, i1, j0, j1; // image dimension (i0 = y_min, i1 = y_max, j0 = x_min, j1 = x_max)
    video_t* video = NULL;
    images_t* images = NULL;
    if (!tools_is_dir(p_in_video)) {
        video = video_init_from_file(p_in_video, p_fra_start, p_fra_end, p_fra_skip, p_ffmpeg_threads, &i0, &i1, &j0,
                                     &j1);
    } else {
        images = images_init_from_path(p_in_video, p_fra_start, p_fra_end, p_fra_skip, p_video_buff);
        i0 = images->i0; i1 = images->i1; j0 = images->j0; j1 = images->j1;
        images->loop_size = (size_t)(p_video_loop);
    }

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    ROI_t* ROI_array_tmp = features_alloc_ROI_array(MAX_ROI_SIZE_BEFORE_SHRINK);
    ROI_t* ROI_array0 = features_alloc_ROI_array(MAX_ROI_SIZE);
    ROI_t* ROI_array1 = features_alloc_ROI_array(MAX_ROI_SIZE);
    vec_BB_t* BB_array = NULL;
    if (p_out_bb)
        BB_array = (vec_BB_t*)vector_create();
    tracking_data_t* tracking_data = tracking_alloc_data(MAX(p_fra_star_min, p_fra_meteor_min), MAX_ROI_SIZE);
    int b = 1; // image border
    uint8_t **I = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // grayscale input image
    uint8_t **IL = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // binary image (after threshold low)
    uint8_t **IH = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // binary image (after threshold high)
    uint32_t **L1 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // labels (CCL)
    uint32_t **L2 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // labels (CCL + hysteresis)
    uint8_t **IT = p_out_frames ? ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b) : NULL; // img tmp to write labels

    // --------------------------- //
    // -- MATRIX INITIALISATION -- //
    // --------------------------- //

    tracking_init_global_data();
    KNN_data_t* knn_data = KNN_alloc_and_init_data(0, MAX_ROI_SIZE, 0, MAX_ROI_SIZE);
    features_init_ROI_array(ROI_array_tmp);
    features_init_ROI_array(ROI_array0);
    features_init_ROI_array(ROI_array1);
    tracking_init_data(tracking_data);
    CCL_data_t* ccl_data = CCL_LSL_alloc_and_init_data(i0, i1, j0, j1);
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
    while ((cur_fra = get_next_frame(video, images, I)) != -1) {
        fprintf(stderr, "(II) Frame n°%4d", cur_fra);

        // step 1: threshold low
        threshold((const uint8_t**)I, IL, i0, i1, j0, j1, p_light_min);

        // step 2: CCL/CCA
        const int n_ROI = CCL_LSL_apply(ccl_data, (const uint8_t**)IL, L1);
        features_extract((const uint32_t**)L1, i0, i1, j0, j1, n_ROI, ROI_array_tmp);

        // step 3: hysteresis threshold & surface filtering (+ magnitude computations)
        threshold((const uint8_t**)I, IH, i0, i1, j0, j1, p_light_max);
        features_merge_CCL_HI_v2((const uint32_t**)L1, (const uint8_t**)IH, L2, i0, i1, j0, j1, ROI_array_tmp,
                                 p_surface_min, p_surface_max);
        features_shrink_ROI_array((const ROI_t*)ROI_array_tmp, ROI_array1);
        features_compute_magnitude((const uint8_t**)I, j1, i1, (const uint32_t**)L2, ROI_array1);

        // step 4: k-NN matching
        KNN_match(knn_data, ROI_array0, ROI_array1, p_k, p_max_dist * p_max_dist);

        // step 5: motion estimation
        double first_theta, first_tx, first_ty, first_mean_error, first_std_deviation;
        double theta, tx, ty, mean_error, std_deviation;
        features_compute_motion((const ROI_t*)ROI_array0, ROI_array1, &first_theta, &first_tx, &first_ty,
                                &first_mean_error, &first_std_deviation, &theta, &tx, &ty, &mean_error, &std_deviation);

        // step 6: tracking
        tracking_perform(tracking_data, (const ROI_t*)ROI_array0, ROI_array1, &BB_array, cur_fra, theta, tx, ty,
                         mean_error, std_deviation, p_r_extrapol, p_angle_max, p_diff_dev, p_track_all, p_fra_star_min,
                         p_fra_meteor_min, p_fra_meteor_max, p_out_mag != NULL);

        // save frames (CCs)
        if (p_out_frames) {
            tools_create_folder(p_out_frames);
            char filename[1024];
            snprintf(filename, sizeof(filename), "%s/%05d.pgm", p_out_frames, cur_fra);
            _tools_save_frame_ui32matrix(filename, (const uint32_t**)L2, i0, i1, j0, j1, IT);
        }

        // save stats
        if (p_out_stats && n_frames) {
            tools_create_folder(p_out_stats);
            char filename[1024];
            snprintf(filename, sizeof(filename), "%s/%05d_%05d.txt", p_out_stats, cur_fra - 1, cur_fra);
            FILE* f = fopen(filename, "w");
            if (f == NULL) {
                fprintf(stderr, "(EE) error while opening '%s'\n", filename);
                exit(1);
            }
            if (f) {
                features_ROI0_ROI1_write(f, cur_fra, ROI_array0, ROI_array1, tracking_data->tracks);
                fprintf(f, "#\n");
                KNN_asso_conflicts_write(f, knn_data, ROI_array0, ROI_array1);
                fprintf(f, "#\n");
                features_motion_write(f, first_theta, first_tx, first_ty, first_mean_error, first_std_deviation, theta,
                                      tx, ty, mean_error, std_deviation);
                fprintf(f, "#\n");
                tracking_track_array_write_full(f, tracking_data->tracks);
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

        ROI_t* tmp = ROI_array0;
        ROI_array0 = ROI_array1;
        ROI_array1 = tmp;
    }
    fprintf(stderr, "\n");

    if (BB_array) {
        FILE* f = fopen(p_out_bb, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_out_bb);
            exit(1);
        }
        tracking_BB_array_write(f, BB_array, tracking_data->tracks);
        fclose(f);
    }

    if (p_out_mag) {
        FILE* f = fopen(p_out_mag, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_out_mag);
            exit(1);
        }
        tracking_track_array_magnitude_write(f, tracking_data->tracks);
        fclose(f);
    }
    tracking_track_array_write(stdout, tracking_data->tracks);

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
    if (IT)
        free_ui8matrix(IT, i0 - b, i1 + b, j0 - b, j1 + b);
    features_free_ROI_array(ROI_array_tmp);
    features_free_ROI_array(ROI_array0);
    features_free_ROI_array(ROI_array1);
    if (video)
        video_free(video);
    if (images)
        images_free(images);
    CCL_LSL_free_data(ccl_data);
    KNN_free_data(knn_data);
    if (BB_array) {
        size_t vs = vector_size(BB_array);
        for (size_t i = 0; i < vs; i++)
            vector_free(BB_array[i]);
        vector_free(BB_array);
    }
    tracking_free_data(tracking_data);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
