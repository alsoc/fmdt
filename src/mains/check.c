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
    char* def_p_trk_path = NULL;
    char* def_p_gt_path = NULL;

    // help
    if (args_find(argc, argv, "--help,-h")) {
        fprintf(stderr, "  --trk-path     Path to tracks file        [%s]\n",
                def_p_trk_path ? def_p_trk_path : "NULL");
        fprintf(stderr, "  --gt-path      Path to ground truth file  [%s]\n",
                def_p_gt_path ? def_p_gt_path : "NULL");
        fprintf(stderr, "  --help, -h     This help                      \n");
        exit(1);
    }

    // Parsing Arguments
    const char* p_trk_path = args_find_char(argc, argv, "--trk-path,--in-tracks", def_p_trk_path);
    const char* p_gt_path = args_find_char(argc, argv, "--gt-path,--in-gt", def_p_gt_path);

    // heading display
    printf("#  --------------------\n");
    printf("# |         ----*      |\n");
    printf("# | --* FMDT-CHECK --* |\n");
    printf("# |   -------*         |\n");
    printf("#  --------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * trk-path = %s\n", p_trk_path);
    printf("#  * gt-path  = %s\n", p_gt_path);
    printf("#\n");

    // arguments checking
    if (!p_trk_path) {
        fprintf(stderr, "(EE) '--trk-path' is missing\n");
        exit(1);
    }
    if (!p_gt_path) {
        fprintf(stderr, "(EE) '--gt-path' is missing\n");
        exit(1);
    }

    tracking_init_global_data();
    vec_track_t tracks;
    tracking_parse_tracks(p_trk_path, &tracks);

    printf("# The program is running...\n");

    // validation pour établir si une track est vrai/faux positif
    validation_init(p_gt_path);
    validation_process(tracks);
    validation_print(tracks);
    validation_free();
    vector_free(tracks);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
