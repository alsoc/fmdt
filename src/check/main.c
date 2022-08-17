/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "args.h"
#include "defines.h"
#include "tracking.h"
#include "validation.h"

int main(int argc, char** argv) {
    // default values
    char* def_input_tracks = NULL;
    char* def_validation = NULL;

    if (args_find(argc, argv, "-h")) {
        fprintf(stderr, "  --input-tracks    Path to tracks file        [%s]\n", def_input_tracks);
        fprintf(stderr, "  --validation      Path to ground truth file  [%s]\n", def_validation);
        fprintf(stderr, "  -h                This help                      \n");
        exit(1);
    }

    // Parsing Arguments
    char* input_tracks = args_find_char(argc, argv, "--input-tracks", def_input_tracks);
    char* validation = args_find_char(argc, argv, "--validation", def_validation);

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

    track_t tracks[MAX_TRACKS_SIZE];
    int n_tracks = 0;
    tracking_init_global_data();
    tracking_init_tracks(tracks, MAX_TRACKS_SIZE);
    tracking_parse_tracks(input_tracks, tracks, &n_tracks);

    printf("# The program is running...\n");

    // validation pour établir si une track est vrai/faux positif
    validation_init(validation);
    validation_process(tracks, n_tracks);
    validation_print(tracks, n_tracks);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
