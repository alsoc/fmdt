/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */ 

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "Tracking.h"
#include "DebugUtil.h"
#include "Validation.h"

#define SIZE_MAX_TRACKS 1000

extern char path_tracks[200];

// ==============================================================================================================================
void main_validation(int argc, char** argv)
// ==============================================================================================================================
{
    // default values
    char* def_input_tracks = NULL;
    char* def_validation   = NULL;

    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "  --input-tracks    Path vers le fichier avec les tracks                [%s]\n", def_input_tracks);
        fprintf(stderr, "  --validation      Fichier contenant la vérité terrain de la séquence  [%s]\n", def_validation  );
        fprintf(stderr, "  -h                This help                                               \n"                  );
        exit(1);
    }

    // Parsing Arguments
    char *src_path   = find_char_arg (argc, argv, "--input-tracks", def_input_tracks);
    char *validation = find_char_arg (argc, argv, "--validation",   def_validation);

    // heading display
    printf("#  ----------------------\n");
    printf("# |         ----*        |\n");
    printf("# | --* METEOR-CHECK --* |\n");
    printf("# |   -------*           |\n");
    printf("#  ----------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * input-tracks = %s\n", src_path);
    printf("#  * validation   = %s\n", validation);
    printf("#\n");

    if (!src_path){
        fprintf(stderr, "(EE) '--input-tracks' is missing\n");
        exit(1);
    }

    if (!validation) {
        fprintf(stderr, "(EE) '--validation' is missing\n");
        exit(1);
    }

    disp(src_path);
    disp(validation);

    Track tracks[SIZE_MAX_TRACKS];
    int nb_tracks = 0;
    init_Track(tracks, SIZE_MAX_TRACKS);
        
    // recupere les tracks
    parseTracks(src_path, tracks, &nb_tracks);
    //printTracks(tracks, nb_tracks);
    unsigned n_tracks = 0, n_stars = 0, n_meteors = 0, n_noise = 0;
    n_tracks = count_objects(tracks, nb_tracks, &n_stars, &n_meteors, &n_noise);

    printf("# Read from tracks file:\n");
    printf("# -> Number of meteor objects: %5d\n", n_meteors);
    printf("# -> Number of star objects:   %5d\n", n_stars);
    printf("# -> Number of noise objects:  %5d\n", n_noise);
    printf("# -> Total number of objects:  %5d\n", n_tracks);
    
    printf("# The program is running...\n");

    // validation pour établir si une track est vrai/faux positif
    Validation_init(validation);
    Validation(tracks, nb_tracks);
    Validation_print();

    printf("# End of the program, exiting.\n");
}

int main(int argc, char** argv)
{
    main_validation(argc, argv);
    
    return 0;
}
