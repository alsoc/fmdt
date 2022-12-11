#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <nrc2.h>

#include "fmdt/args.h"
#include "fmdt/defines.h"
#include "fmdt/tools.h"
#include "fmdt/video.h"

int main(int argc, char** argv) {
    // default values
    char* def_p_in_video = NULL;
    char* def_p_out_frames = NULL;
    int def_p_fra_start = 0;
    int def_p_fra_end = 0;
    int def_p_ffmpeg_threads = 0;

    // help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr, "  --in-video        Video source                             [%s]\n",
                def_p_in_video ? def_p_in_video : "NULL");
        fprintf(stderr, "  --out-frames      Path to the directory of output frames   [%s]\n",
                def_p_out_frames ? def_p_out_frames : "NULL");
        fprintf(stderr, "  --fra-start       Starting frame in the video              [%d]\n", def_p_fra_start);
        fprintf(stderr, "  --fra-end         Ending frame in the video                [%d]\n", def_p_fra_end);
        fprintf(stderr, "  --ffmpeg-threads  Select the number of threads to use to   [%d]\n"
                        "                    decode video input (in ffmpeg)               \n", def_p_ffmpeg_threads);
        fprintf(stderr, "  -h                This help                                    \n");
        exit(1);
    }

    // Parsing Arguments
    const char* p_in_video = args_find_char(argc, argv, "--in-video", def_p_in_video);
    const char* p_out_frames = args_find_char(argc, argv, "--out-frames", def_p_out_frames);
    const int p_fra_start = args_find_int(argc, argv, "--fra-start", def_p_fra_start);
    const int p_fra_end = args_find_int(argc, argv, "--fra-end", def_p_fra_end);
    const int p_ffmpeg_threads = args_find_int(argc, argv, "--ffmpeg-threads", def_p_ffmpeg_threads);

    // heading display
    printf("#  ----------------------\n");
    printf("# |         ----*        |\n");
    printf("# | --* FMDT-VID2IMG --* |\n");
    printf("# |   -------*           |\n");
    printf("#  ----------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * in-video       = %s\n", p_in_video);
    printf("#  * out-frames     = %s\n", p_out_frames);
    printf("#  * fra-start      = %d\n", p_fra_start);
    printf("#  * fra-end        = %d\n", p_fra_end);
    printf("#  * ffmpeg-threads = %d\n", p_ffmpeg_threads);
    printf("#\n");

    // arguments checking
    if (!p_in_video) {
        fprintf(stderr, "(EE) '--in-video' is missing\n");
        exit(1);
    }
    if (!p_out_frames) {
        fprintf(stderr, "(EE) '--out-frames' is missing\n");
        exit(1);
    }
    if (p_fra_end && p_fra_end < p_fra_start) {
        fprintf(stderr, "(EE) '--fra-end' has to be higher than '--fra-start'\n");
        exit(1);
    }
    if (p_ffmpeg_threads < 0) {
        fprintf(stderr, "(EE) '--ffmpeg-threads' has to be bigger or equal to 0\n");
        exit(1);
    }

    printf("# The program is running...\n");

    // ------------------------- //
    // -- INITIALISATION VIDEO-- //
    // ------------------------- //
    // sequence
    const int skip = 0;
    // image
    // int b = 1;
    int i0, i1, j0, j1;
    video_t* video = video_init_from_file(p_in_video, p_fra_start, p_fra_end, skip, p_ffmpeg_threads, &i0, &i1, &j0,
                                          &j1);

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //
    uint8_t** img = (uint8_t**)ui8matrix(i0, i1, j0, j1);

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//
    tools_create_folder(p_out_frames);
    int frame;
    while ((frame = video_get_next_frame(video, img)) != -1) {
        fprintf(stderr, "(II) Frame n°%4d\r", frame);
        char filename[1024];
        snprintf(filename, sizeof(filename), "%s/%05u.pgm", p_out_frames, frame);
        SavePGM_ui8matrix(img, i0, i1, j0, j1, filename);
    }
    fprintf(stderr, "\n");

    // ----------
    // -- free --
    // ----------
    free_ui8matrix(img, i0, i1, j0, j1);
    video_free(video);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
