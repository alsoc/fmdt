#include "nrutil.h"
#include "nrtype.h"
#include "Args.h"
#include "Video.h"
#include "CCL.h"
#include "Features.h"
#include "Threshold.h"
#include "DebugUtil.h"
#include "macro_debug.h"
#include "KPPV.h"
#include "Tracking.h"
#include "Ballon.h"
#include "Validation.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define SEQUENCE_DST_PATH_HIST "hist/"
#define SEQUENCE_NDIGIT 5
#define K 3

#define SIZE_MAX_METEORROI 20000
#define SIZE_MAX_TRACKS 1000
#define SIZE_MAX_KPPV 200

/*DEBUG*/
extern char path_stats_0[200];
extern char path_stats_1[200];
extern char path_frames_binary[250];
extern char path_frames_output[250];
extern char path_motion[200];
extern char path_extraction[200];
extern char path_error[200];
extern char path_tracks[200];
extern char path_bounding_box[200];
extern char path_debug[150];
extern char path_video_tracking[200];
extern uint32 *conflicts;
extern uint32 **nearest;
extern float32 **distances; 
extern elemBB *tabBB[NB_FRAMES];


// NON Testé depuis les dernieres modifs (à check)
// ======================================
void meteor_ballon_hyst_frame(int argc, char** argv)
// ======================================
{
    // Parsing Arguments
    int start          = find_int_arg  (argc, argv, "-start_frame",     1 );
    int end            = find_int_arg  (argc, argv, "-end_frame",     1000);
    int light_min      = find_int_arg  (argc, argv, "-light_min",      60 ); // a definir
    int light_max      = find_int_arg  (argc, argv, "-light_max",      85 ); // a definir
    int surface_min    = find_int_arg  (argc, argv, "-surface_min",      3); // a definir
    int surface_max    = find_int_arg  (argc, argv, "-surface_max",    500); // a definir
    int k              = find_int_arg  (argc, argv, "-k",                3); // a definir
    int r_extrapol     = find_int_arg  (argc, argv, "-r_extrapol",       5); // a definir
    int d_line         = find_int_arg  (argc, argv, "-d_line",          25); // a definir
    int diff_deviation = find_int_arg  (argc, argv, "-diff_deviation",   4); // a definir
    char* src_path     = find_char_arg (argc, argv, "-input",      NULL);
    char* dest_path    = find_char_arg (argc, argv, "-output",     NULL);
    int debug          = find_arg      (argc, argv, "-debug");

    if(!src_path){
        printf("Input missing\n");
        exit(1);
    }
    if(!dest_path){
        printf("Output missing -> no video/frames will be saved\n");
    }

    // sequence
    char *filename;
    char src[100];
	double theta, tx, ty;
    int frame = start;

    // CC
	MeteorROI stats0[SIZE_MAX_METEORROI];
	MeteorROI stats1[SIZE_MAX_METEORROI];
	MeteorROI stats_shrink[SIZE_MAX_METEORROI];
    Track tracks[10000];

    int  offset =  0;
    int  last   = -1;


	int n0 = 0;
	int n1 = 0;

    // image
    int b = 1;                  
    // TRES MOCHE 
    int i0 = 0, i1 = 1200 , j0 = 0, j1 = 1900;

    // //path management
    char *slash = src_path, *next;
    while ((next = strpbrk(slash + 1, "\\/"))) {
        slash = next;
    }
    if (src_path != slash) slash++;
    filename = strndup(slash, next-slash);

  	create_debug_dir (filename, light_min , light_max, -1);
	if (dest_path) create_frames_dir(dest_path, filename, light_min , light_max, -1);
    
    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    // struct for image processing
    Ballon* ballon = allocBallon(i0, i1, j0, j1, b);

    // -------------------------- //
    // -- INITIALISATION MATRIX-- //
    // -------------------------- //

    initBallon(ballon, i0, i1, j0, j1, b);
    initTabBB();

	kppv_init(0, SIZE_MAX_KPPV, 0, SIZE_MAX_KPPV);
	init_MeteorROI(stats0, SIZE_MAX_METEORROI);
	init_MeteorROI(stats1, SIZE_MAX_METEORROI);
    init_Track(tracks, SIZE_MAX_TRACKS);
    CCL_LSL_init(i0, i1, j0, j1);

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//

    while(frame <= end) {        
        sprintf(src, "%s%03d.pgm", src_path, frame); 
        disp(src);
        MLoadPGM_ui8matrix(src, i0, i1, j0, j1, ballon->I0);
		printf("[Frame] %-4d\n", frame-1);

        
		//---------------------------------------------------------//
        PUTS("\t Step 1 : seuillage low/high");
        copy_ui8matrix_ui8matrix(ballon->I0, i0, i1, j0, j1, ballon->SH); 
        copy_ui8matrix_ui8matrix(ballon->I0, i0, i1, j0, j1, ballon->SM);

        threshold_high(ballon->SM, i0, i1, j0, j1, light_min);
        threshold_high(ballon->SH, i0, i1, j0, j1, light_max);
     	//---------------------------------------------------------//
        convert_ui8matrix_ui32matrix(ballon->SM, i0, i1, j0, j1, ballon->SM32);
        convert_ui8matrix_ui32matrix(ballon->SH, i0, i1, j0, j1, ballon->SH32);
     	//--------------------------------------------------------//
        PUTS("\t Step 2 : ECC/ACC");
        n1 = CCL_LSL(ballon->SM32, i0, i1, j0, j1);
        extract_features(ballon->SM32, i0, i1, j0, j1, stats1, n1);
        idisp(n1);
     	//--------------------------------------------------------//
        PUTS("\t Step 3 : seuillage hysteresis && filtrage surfacique"); 
        merge_HI_CCL_v2(ballon->SH32, ballon->SM32, i0, i1, j0, j1, stats1, n1, surface_min, surface_max );
        int n_shrink = shrink_stats(stats1, stats_shrink, n1);

      	//--------------------------------------------------------//
        PUTS("\t Step 4 : mise en correspondance");
		kppv_routine(stats0, stats_shrink, n0, n_shrink, k);

      	//--------------------------------------------------------//
        PUTS("\t Step 5 : recalage");
        motion(stats0, stats_shrink, n0, n_shrink, &theta, &tx, &ty);

        PUTS("\t Step 6: Tracking");
        Tracking(stats0, stats_shrink, tracks, n0, n_shrink, frame, &last, &offset, theta, tx, ty, r_extrapol, d_line, diff_deviation);
        
        //--------------------------------------------------------//
        PUTS("\t [DEBUG] Saving frames");
        if (dest_path){
	        create_frames_files(frame);
            saveFrame_ui32matrix(path_frames_binary, ballon->SH32, i0, i1, j0, j1);
            // saveFrame_ui8matrix(path_frames_binary, ballon->I0, i0, i1, j0, j1);
        }

        PUTS("\t [DEBUG] Saving stats");
        if (debug){
	        create_debug_files (frame);
            saveAssoConflicts(path_debug, frame-1, conflicts, nearest, distances, n0, n_shrink, stats0, stats_shrink); 
            // saveMotion(path_motion, theta, tx, ty, frame-1);
            // saveMotionExtraction(path_extraction, stats0, stats_shrink, n0, theta, tx, ty, frame-1);
            // saveError(path_error, stats0, n0);
        }
      	//--------------------------------------------------------//
        SWAP_STATS(stats0, stats_shrink, n_shrink);
        n0 = n_shrink;
        frame++;
    }
    saveTracks(path_tracks, tracks, last);
    // printTracks(tracks, last);

    // ----------
    // -- free --
    // ----------

    freeBallon(ballon, i0, i1, j0, j1, b);

    CCL_LSL_free(i0, i1, j0, j1);
	kppv_free(0, 50, 0, 50);
}

// ======================================
void meteor_ballon_hyst(int argc, char** argv)
// ======================================
{
    // Help
    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "  -input        : Video source\n");
        fprintf(stderr, "  -output       : path frames output\n");
        fprintf(stderr, "  -start_frame  : Image de départ dans la séquence\n");
        fprintf(stderr, "  -end_frame    : Dernière image de la séquence\n");
        fprintf(stderr, "  -skip_frames  : Nombre d'images à sauter\n");
        fprintf(stderr, "  -light_min    : Seuil bas filtrage lumineux\n");
        fprintf(stderr, "  -light_max    : Seuil haut filtrage lumineux\n");
        fprintf(stderr, "  -surface_min  : Surface max des CC en pixels\n");
        fprintf(stderr, "  -surface_max  : Surface min des CC en pixels\n");
        fprintf(stderr, "  -k            : Le nombre de voisins dans KPPV\n");
        fprintf(stderr, "  -r_extrapol   : Le rayon de recherche d'une CC dans le cas d'une extrapolation\n");
        fprintf(stderr, "  -d_line       : Le delta pour lequel un point est toujours considéré comme étant sur une droite\n");
        fprintf(stderr, "  -diff_deviaton: Le facteur de multiplication de l'ecart type (l'erreur d'une CC doit etre superieure a diff_deviation*ecart_type pour etre considéré en mouvement \n");
        fprintf(stderr, "  -validation   : Fichier contenant la vérité terrain de la séquence\n");
        fprintf(stderr, "  -debug        : save files debug\n");
        exit(1);
    }

    // Parsing Arguments
    int start          = find_int_arg  (argc, argv, "-start_frame",     1 );
    int end            = find_int_arg  (argc, argv, "-end_frame",     1000);
    int skip         = find_int_arg  (argc, argv, "-skip_frames", 0 );
    int light_min      = find_int_arg  (argc, argv, "-light_min",      60 ); // a definir
    int light_max      = find_int_arg  (argc, argv, "-light_max",      85 ); // a definir
    int surface_min    = find_int_arg  (argc, argv, "-surface_min",      3); // a definir
    int surface_max    = find_int_arg  (argc, argv, "-surface_max",    500); // a definir
    int k              = find_int_arg  (argc, argv, "-k",                3); // a definir
    int r_extrapol     = find_int_arg  (argc, argv, "-r_extrapol",       5); // a definir
    int d_line         = find_int_arg  (argc, argv, "-d_line",          25); // a definir
    int diff_deviation = find_int_arg  (argc, argv, "-diff_deviation",   4); // a definir
    char* src_path     = find_char_arg (argc, argv, "-input",      NULL);
    char* dest_path    = find_char_arg (argc, argv, "-output",     NULL);
    char *validation = find_char_arg (argc, argv, "-validation", NULL);
    int debug          = find_arg      (argc, argv, "-debug");
    if(!src_path){
        printf("Input missing\n");
        exit(1);
    }
    if(!dest_path){
        printf("Output missing -> no frames will be saved\n");
    }

    // sequence
    char *filename;
	double theta, tx, ty;
    int frame;

    // CC
	MeteorROI stats0[SIZE_MAX_METEORROI];
	MeteorROI stats1[SIZE_MAX_METEORROI];
	MeteorROI stats_shrink[SIZE_MAX_METEORROI];
    Track tracks[SIZE_MAX_TRACKS];
    Track tracks_stars[SIZE_MAX_TRACKS];

    int  offset =  0;
    int  last   = -1;

	int n0 = 0;
	int n1 = 0;

    // image
    int b = 1;                  
    int i0, i1, j0, j1;

    //path management
    char *path;
    split_path_file(&path, &filename, src_path);
  	create_debug_dir (filename, light_min , light_max, -1);
	if (dest_path) create_frames_dir(dest_path, filename, light_min , light_max, -1);

    // ------------------------- //
    // -- INITIALISATION VIDEO-- //
    // ------------------------- //
    PUTS("INIT VIDEO");
    Video* video = Video_init_from_file(src_path, start, end, skip, &i0, &i1, &j0, &j1);
    
    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //
    PUTS("ALLOC");

    // struct for image processing
    Ballon* ballon = allocBallon(i0, i1, j0, j1, b);

    // -------------------------- //
    // -- INITIALISATION MATRIX-- //
    // -------------------------- //

    initBallon(ballon, i0, i1, j0, j1, b);
	kppv_init(0, SIZE_MAX_KPPV, 0, SIZE_MAX_KPPV);
	init_MeteorROI(stats0, SIZE_MAX_METEORROI);
	init_MeteorROI(stats1, SIZE_MAX_METEORROI);
    init_Track(tracks, SIZE_MAX_TRACKS);
    init_Track(tracks_stars, SIZE_MAX_TRACKS);
    CCL_LSL_init(i0, i1, j0, j1);
    initTabBB();

    if (validation) 
        Validation(validation,tracks, SIZE_MAX_TRACKS, "./debug/");

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//

    PUTS("LOOP");
	if(!Video_nextFrame(video,ballon->I0)) { 
        exit(1);
    }

    while(Video_nextFrame(video,ballon->I1)) {
        
        frame = video->frame_current-2;
		printf("[Frame] %-4d\n", frame);

		//---------------------------------------------------------//
        PUTS("\t Step 1 : seuillage low/high");
        copy_ui8matrix_ui8matrix(ballon->I0, i0, i1, j0, j1, ballon->SH); 
        copy_ui8matrix_ui8matrix(ballon->I0, i0, i1, j0, j1, ballon->SM);
		//---------------------------------------------------------//
        threshold_high(ballon->SM, i0, i1, j0, j1, light_min);
        threshold_high(ballon->SH, i0, i1, j0, j1, light_max);
     	//---------------------------------------------------------//
        convert_ui8matrix_ui32matrix(ballon->SM, i0, i1, j0, j1, ballon->SM32); 
        convert_ui8matrix_ui32matrix(ballon->SH, i0, i1, j0, j1, ballon->SH32);

     	//--------------------------------------------------------//
        PUTS("\t Step 2 : ECC/ACC");
        n1 = CCL_LSL(ballon->SM32, i0, i1, j0, j1);
        idisp(n1);
        extract_features(ballon->SM32, i0, i1, j0, j1, stats1, n1);

     	//--------------------------------------------------------//
        PUTS("\t Step 3 : seuillage hysteresis && filter surface"); 
        merge_HI_CCL_v2(ballon->SH32, ballon->SM32, i0, i1, j0, j1, stats1, n1, surface_min, surface_max); 
        int n_shrink = shrink_stats(stats1, stats_shrink, n1);

      	//--------------------------------------------------------//
        PUTS("\t Step 4 : mise en correspondance");
		kppv_routine(stats0, stats_shrink, n0, n_shrink, k);

      	//--------------------------------------------------------//
        PUTS("\t Step 5 : recalage");
        motion(stats0, stats_shrink, n0, n_shrink, &theta, &tx, &ty);

      	//--------------------------------------------------------//
        PUTS("\t Step 6: Tracking");
        Tracking(stats0, stats_shrink, tracks, n0, n_shrink, frame, &last, &offset, theta, tx, ty, r_extrapol, d_line, diff_deviation);
        // TrackStars(stats0, stats_shrink, tracks, n0, n_shrink, frame, &last, &offset); // Pour matric ROC
        
        //--------------------------------------------------------//
        PUTS("\t [DEBUG] Saving frames");
        if (dest_path){
	        create_frames_files(frame);
            saveFrame_ui32matrix(path_frames_binary, ballon->SH32, i0, i1, j0, j1);
            // saveFrame_ui8matrix(path_frames_binary, ballon->I0, i0, i1, j0, j1);
        }

        PUTS("\t [DEBUG] Saving stats");
        if (debug){
    	    create_debug_files (frame);
            saveAssoConflicts(path_debug, frame-1, conflicts, nearest, distances, n0, n_shrink, stats0, stats_shrink); 
            // saveMotion(path_motion, theta, tx, ty, frame-1);
            // saveMotionExtraction(path_extraction, stats0, stats_shrink, n0, theta, tx, ty, frame-1);
            // saveError(path_error, stats0, n0);
        }

      	//--------------------------------------------------------//
        SWAP_UI8(ballon->I0, ballon->I1);
        SWAP_STATS(stats0, stats_shrink, n_shrink);
        n0 = n_shrink;
    }
    
    saveTabBB(path_bounding_box, tabBB, NB_FRAMES);
    saveTracks(path_tracks, tracks, last);

    if (validation) 
        Validation_final();


    if (debug) printf("Files for debug saved in %s \n", path_debug);
    if (dest_path) printf("Frames saved in %s \n", dest_path);
    // ----------
    // -- free --
    // ----------

    freeBallon(ballon, i0, i1, j0, j1, b);

    free(path);
    free(filename);
    Video_free(video);
    CCL_LSL_free(i0, i1, j0, j1);
	kppv_free(0, 50, 0, 50);
    if (validation) 
        Validation_free();
}


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

// ======================================
void meteor_ballon_max(int argc, char** argv)
// ======================================
{
    // output -> max.pgm
    // Help
    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "  -input        : Video source\n");
        fprintf(stderr, "  -start_frame  : Image de départ dans la séquence\n");
        fprintf(stderr, "  -end_frame    : Dernière image de la séquence\n");
        fprintf(stderr, "  -skip_frames  : Nombre d'images à sauter\n");
        exit(1);
    }

    // Parsing Arguments
    int start        = find_int_arg  (argc, argv, "-start_frame", 0 );
    int end          = find_int_arg  (argc, argv, "-end_frame", 1000);
    int skip         = find_int_arg  (argc, argv, "-skip_frames", 0 );
    char* src_path   = find_char_arg (argc, argv, "-input",      NULL);

    // sequence
    int frame;

    // image
    int b = 1;                  
    int i0, i1, j0, j1;

    // ------------------------- //
    // -- INITIALISATION VIDEO-- //
    // ------------------------- //
    PUTS("INIT VIDEO");
    Video* video = Video_init_from_file(src_path, start, end, skip, &i0, &i1, &j0, &j1);
    
    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //
    PUTS("ALLOC");
    Ballon* ballon = allocBallon(i0, i1, j0, j1, b);

    // -------------------------- //
    // -- INITIALISATION MATRIX-- //
    // -------------------------- //
    initBallon(ballon, i0, i1, j0, j1, b);

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//
    PUTS("LOOP");
	// if(!Video_nextFrame(video,ballon->I0)) { 
        // exit(1);
    // }

    while(Video_nextFrame(video,ballon->I1)) {
        frame = video->frame_current - 1;
		printf("[Frame] %-4d\n", frame);
        max_accumulate(ballon->I0, i0, i1, j0, j1, ballon->I1);
    
    }

    SavePGM_ui8matrix(ballon->I0, i0, i1, j0, j1, "max.pgm");
    
    // ----------
    // -- free --
    // ----------
    freeBallon(ballon, i0, i1, j0, j1, b);
    Video_free(video);
}



int main(int argc, char** argv)
{
    // meteor_ballon_hyst_frame(argc, argv); // 
    meteor_ballon_hyst(argc, argv); // 
    // meteor_ballon_max(argc, argv); // 

    return 0;
}





