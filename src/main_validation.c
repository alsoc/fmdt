/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */ 

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "nrutil.h"
#include "Tracking.h"
#include "DebugUtil.h"
#include "Validation.h"

#define SIZE_MAX_METEORROI 3000
#define SIZE_MAX_TRACKS 1000
#define TOLERANCE_DISTANCEMIN 20

extern char path_tracks[200];

// ==============================================================================================================================
void test_validation(int argc, char** argv)
// ==============================================================================================================================
{

    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "  -input_tracks : Path vers le fichier avec les tracks\n");
        fprintf(stderr, "  -output       : Output de la video\n");
        fprintf(stderr, "  -validation   : Fichier contenant la vérité terrain de la séquence\n");
        exit(1);
    }

    // Parsing Arguments
    char *src_path          = find_char_arg (argc, argv, "-input_tracks",   NULL);
    char *dest_path         = find_char_arg (argc, argv, "-output",         ".");
    char *validation        = find_char_arg (argc, argv, "-validation",     NULL);

    int light_min, light_max;
    char *filename;

    if (!src_path){
        printf("Input(s) missing\n");
        exit(1);
    }

    if (!validation) {
        printf("Validation missing\n");
        exit(1);
    }

    disp(src_path);
    disp(dest_path);
    disp(validation);
    get_data_from_tracks_path(src_path, &light_min, &light_max, &filename);

    Track tracks[SIZE_MAX_TRACKS];
    int nb_tracks = 0;
	init_Track(tracks, SIZE_MAX_TRACKS);
        
    // recupere les tracks
    parseTracks(src_path, tracks, &nb_tracks);
    printTracks(tracks, nb_tracks);
    
    // validation pour établir si une track est vrai/faux positif
    Validation_init(validation);
    Validation(tracks, nb_tracks);
    Validation_save(dest_path, filename);

    free(filename);
}

int main(int argc, char** argv)
{
    test_validation(argc, argv);
    
    return 0;
}
