#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fmdt/args.h"
#include "vec.h"

#include "fmdt/tracking/tracking_global.h"
#include "fmdt/tracking/tracking_io.h"
#include "fmdt/validation.h"

int main(int argc, char** argv) {
    // default values
    char* def_p_in_tracks = NULL;
    char* def_p_in_gt = NULL;

    // help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr, "  --in-tracks    Path to tracks file        [%s]\n",
                def_p_in_tracks ? def_p_in_tracks : "NULL");
        fprintf(stderr, "  --in-gt        Path to ground truth file  [%s]\n",
                def_p_in_gt ? def_p_in_gt : "NULL");
        fprintf(stderr, "  -h             This help                      \n");
        exit(1);
    }

    // Parsing Arguments
    const char* p_in_tracks = args_find_char(argc, argv, "--in-tracks", def_p_in_tracks);
    const char* p_in_gt = args_find_char(argc, argv, "--in-gt", def_p_in_gt);

    // heading display
    printf("#  --------------------\n");
    printf("# |         ----*      |\n");
    printf("# | --* FMDT-CHECK --* |\n");
    printf("# |   -------*         |\n");
    printf("#  --------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * in-tracks = %s\n", p_in_tracks);
    printf("#  * in-gt     = %s\n", p_in_gt);
    printf("#\n");

    // arguments checking
    if (!p_in_tracks) {
        fprintf(stderr, "(EE) '--in-tracks' is missing\n");
        exit(1);
    }
    if (!p_in_gt) {
        fprintf(stderr, "(EE) '--in-gt' is missing\n");
        exit(1);
    }

    tracking_init_global_data();
    vec_track_t track_array;
    tracking_parse_tracks(p_in_tracks, &track_array);

    printf("# The program is running...\n");

    // validation pour établir si une track est vrai/faux positif
    validation_init(p_in_gt);
    validation_process(track_array);
    validation_print(track_array);
    validation_free();
    vector_free(track_array);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
