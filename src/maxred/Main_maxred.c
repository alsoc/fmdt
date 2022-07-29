/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */ 

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "DebugUtil.h"

// ---------------------------------------------------------------------
void max_accumulate(uint8**M, int i0, int i1, int j0, int j1, uint8** I)
// ---------------------------------------------------------------------
{
    for(int i = i0; i <= i1; i++) {
        for(int j = j0; j <= j1; j++) {
            uint8 x = I[i][j];
            uint8 m = M[i][j];
            if(x > m) {
                M[i][j] = x;
            }
        }
    }
}

// ==============================================================================================================================
void main_maxred(int argc, char** argv)
// ==============================================================================================================================
{
    // default values
    char* def_input_video    =    NULL;
    char* def_output_frame   =    NULL;
    int   def_start_frame    =       0;
    int   def_end_frame      =  200000;

    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "  --input-video     Video source                                        [%s]\n", def_input_video);
        fprintf(stderr, "  --output-frame    Path to the frames output                           [%s]\n", def_output_frame);
        fprintf(stderr, "  --start-frame     Starting point of the video                         [%d]\n", def_start_frame);
        fprintf(stderr, "  --end-frame       Ending point of the video                           [%d]\n", def_end_frame);
        fprintf(stderr, "  -h                This help                                               \n"               );
        exit(1);
    }

    // Parsing Arguments
    char* input_video      = find_char_arg (argc, argv, "--input-video",    def_input_video   );
    char *dest_path_frame  = find_char_arg (argc, argv, "--output-frame",  def_output_frame   );
    int start              = find_int_arg  (argc, argv, "--start-frame",    def_start_frame   );
    int end                = find_int_arg  (argc, argv, "--end-frame",      def_end_frame     );

    // heading display
    printf("#  -----------------------\n");
    printf("# |         ----*         |\n");
    printf("# | --* METEOR-MAXRED --* |\n");
    printf("# |   -------*            |\n");
    printf("#  -----------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * input-video   = %s\n",input_video);
    printf("#  * output-frame = %s\n", dest_path_frame);
    printf("#\n");

    if (!input_video){
        fprintf(stderr, "(EE) '--input-video' is missing\n");
        exit(1);
    }

    if (!dest_path_frame) {
        fprintf(stderr, "(EE) '--output-frame' is missing\n");
        exit(1);
    }

    printf("# The program is running...\n");

    disp(input_video);
    disp(dest_path_frame);

    // sequence
    int frame;
    int skip = 0;

    // image
    int b = 1;                  
    int i0, i1, j0, j1;

    // ------------------------- //
    // -- INITIALISATION VIDEO-- //
    // ------------------------- //
    PUTS("INIT VIDEO");
    Video* video = Video_init_from_file(input_video, start, end, skip, &i0, &i1, &j0, &j1);

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //
    PUTS("ALLOC");
    uint8 **img = ui8matrix(i0, i1, j0, j1);
    uint8 **Max = ui8matrix(i0, i1, j0, j1);

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//
    PUTS("LOOP");
    while(Video_nextFrame(video,img)) {
        frame = video->frame_current - 1;
		printf("[Frame] %-4d\n", frame);
        max_accumulate(Max, i0, i1, j0, j1, img);
    }

    SavePGM_ui8matrix(Max, i0, i1, j0, j1, "max.pgm");

    // ----------
    // -- free --
    // ----------
    free_ui8matrix(img, i0, i1, j0, j1);
    free_ui8matrix(Max, i0, i1, j0, j1);
    Video_free(video);

    printf("# End of the program, exiting.\n");
}

int main(int argc, char** argv)
{
    main_maxred(argc, argv);
    
    return 0;
}
