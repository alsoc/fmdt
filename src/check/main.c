/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "args.h"
#include "debug_utils.h"
#include "tracking.h"
#include "validation.h"

#define SIZE_MAX_TRACKS 1000

void main_validation(int argc, char** argv) {
    // default values
    char* def_input_tracks = NULL;
    char* def_validation = NULL;

    if (args_find_arg(argc, argv, "-h")) {
        fprintf(stderr, "  --input-tracks    Path to tracks file        [%s]\n", def_input_tracks);
        fprintf(stderr, "  --validation      Path to ground truth file  [%s]\n", def_validation);
        fprintf(stderr, "  -h                This help                      \n");
        exit(1);
    }

    // Parsing Arguments
    char* input_tracks = args_find_char_arg(argc, argv, "--input-tracks", def_input_tracks);
    char* validation = args_find_char_arg(argc, argv, "--validation", def_validation);

    // heading display
    printf("#  ----------------------\n");
    printf("# |         ----*        |\n");
    printf("# | --* METEOR-CHECK --* |\n");
    printf("# |   -------*           |\n");
    printf("#  ----------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * input-tracks = %s\n", input_tracks);
    printf("#  * validation   = %s\n", validation);
    printf("#\n");

    if (!input_tracks) {
        fprintf(stderr, "(EE) '--input-tracks' is missing\n");
        exit(1);
    }

    if (!validation) {
        fprintf(stderr, "(EE) '--validation' is missing\n");
        exit(1);
    }

    track_t tracks[SIZE_MAX_TRACKS];
    int nb_tracks = 0;
    tracking_init_global_data();
    tracking_init_tracks(tracks, SIZE_MAX_TRACKS);

    // recupere les tracks
    tracking_parse_tracks(input_tracks, tracks, &nb_tracks);

    printf("# The program is running...\n");

    // validation pour établir si une track est vrai/faux positif
    validation_init(validation);
    validation_process(tracks, nb_tracks);
    validation_print(tracks, nb_tracks);

    printf("# End of the program, exiting.\n");
}

int main(int argc, char** argv) {
    main_validation(argc, argv);
    return 0;
}
