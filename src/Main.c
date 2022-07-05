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

#define SIZE_MAX_METEORROI 15000
#define SIZE_MAX_TRACKS 1000
#define SIZE_MAX_KPPV 200

/*DEBUG*/
extern char path_stats_0[200];
extern char path_stats_1[200];
extern char path_frame[200];
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


/*
// ======================================
void meteor_tau_h(int argc, char** argv)
// ======================================
{
    printf("YOYOYO\n");
    PUTS("ALLO");
    // Help
    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "usage: %s %s [options] <input_file>\n", argv[0], argv[1]);
        fprintf(stderr, "  -start_frame  : Image de départ dans la séquence\n");
        fprintf(stderr, "  -end_frame    : Dernière image de la séquence\n");
        fprintf(stderr, "  -skip_frames  : Nombre d'images à sauter\n");
        fprintf(stderr, "  -light        : Seuil filtrage lumineux\n");
        fprintf(stderr, "  -light_min    : Seuil filtrage lumineux\n");
        fprintf(stderr, "  -light_max    : Seuil filtrage lumineux\n");
        fprintf(stderr, "  -surface_min  : Surface max des CC en pixels\n");
        fprintf(stderr, "  -surface_max  : Surface min des CC en pixels\n");
        fprintf(stderr, "  -validation   : Fichier contenant la vérité terrain de la séquence\n");
        exit(1);
    }

    // Parsing Arguments
    int start        = find_int_arg  (argc, argv, "-start_frame", 1);
    int end          = find_int_arg  (argc, argv, "-end_frame", 1000);
    int skip         = find_int_arg  (argc, argv, "-skip_frames", 0 );
    int light_min    = find_int_arg  (argc, argv, "-light_min",  50 ); // a definir
    int light_max    = find_int_arg  (argc, argv, "-light_max",  75 ); // a definir
    int surface_min  = find_int_arg  (argc, argv, "-surface_min", 8); // a definir
    int surface_max  = find_int_arg  (argc, argv, "-surface_max",300); // a definir
    char* src_path   = find_char_arg (argc, argv, "-input",      NULL);
    char* validation = find_char_arg (argc, argv, "-validation", NULL);

    // sequence
    char *filename;
    char debug[100] = "./debug";
    char frames[100];
    char dest[100];
    char src[100];

    int frame = start;


    PUTS("CC");

    // CC
	MeteorROI stats0[SIZE_MAX_METEORROI];
	MeteorROI stats1[SIZE_MAX_METEORROI];
	MeteorROI stats2[SIZE_MAX_METEORROI];
	MeteorROI stats3[SIZE_MAX_METEORROI];
	MeteorROI stats1_shrink[SIZE_MAX_METEORROI];
	MeteorROI stats3_shrink[SIZE_MAX_METEORROI];
    Track tracks[1000];
    Track tracks_stars[1000];

    int  offset =  0;
    int  last   = -1;

	int n0 = 0;
	int n1 = 0;
	int n2 = 0;
	int n3 = 0;

    // image
    int b = 1;                  
    // TRES MOCHE 
    int i0 = 0, i1 = 1200-1 , j0 = 0, j1 = 1920-1;
    double theta, tx, ty;
    
    PUTS("path management");

    //path management
    char *slash = src_path, *next;
    while ((next = strpbrk(slash + 1, "\\/"))) {
        slash = next;
    }
    if (&src_path != slash) slash++;
    filename = strndup(slash, next-slash);


  	create_debug_dir (filename,  light_min, light_max, -1);
	create_frames_dir(filename,  light_min, light_max, -1);
    
    // ------------------------- //
    // -- INITIALISATION -- //
    // ------------------------- //
    
    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    PUTS("Alloc");

    // struct for image processing
    Ballon* ballon = allocBallon(i0, i1, j0, j1, b);

    // -------------------------- //
    // -- INITIALISATION MATRIX-- //
    // -------------------------- //
    PUTS("init");
    initBallon(ballon, i0, i1, j0, j1, b);
	kppv_init(0, SIZE_MAX_KPPV, 0, SIZE_MAX_KPPV);
	init_MeteorROI(stats0, SIZE_MAX_METEORROI);
	init_MeteorROI(stats1, SIZE_MAX_METEORROI);
	init_MeteorROI(stats2, SIZE_MAX_METEORROI);
	init_MeteorROI(stats3, SIZE_MAX_METEORROI);
	// init_MeteorROI(stats1_shrink, SIZE_MAX_METEORROI);
	// init_MeteorROI(stats3_shrink, SIZE_MAX_METEORROI);
    init_Track(tracks, SIZE_MAX_TRACKS);
    init_Track(tracks_stars, SIZE_MAX_TRACKS);
    CCL_LSL_init(i0, i1, j0, j1);

    PUTS("Traitement");

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//

    while ( frame <= end ) {
        sprintf(src, "%s/%04d.pgm", src_path, frame);
        MLoadPGM_ui8matrix(src, i0, i1, j0, j1, ballon->I1);
        printf("[Frame] %-4d\n", frame);


	    create_debug_files (frame);
	    create_frames_files(frame);



        PUTS("\t Step 1 : Seuillage très haut");
        copy_ui8matrix_ui8matrix(ballon->I1, i0, i1, j0, j1, ballon->SH);

        threshold_high(ballon->SH, i0, i1, j0, j1, 100);
        convert_ui8matrix_ui32matrix(ballon->SH, i0, i1, j0, j1, ballon->SH32);
        
     	//--------------------------------------------------------//
        PUTS("\t Step 2 : ECC/ACC");
        n1 = CCL_LSL(ballon->SH32, i0, i1, j0, j1);
        extract_features(ballon->SH32, i0, i1, j0, j1, stats1, n1);
            
      	//--------------------------------------------------------//
        PUTS("\t Step 3 : Filter surface");
        filter_surface(stats1, n1, ballon->SH32, surface_min, surface_max);
        int n1_shrink = shrink_stats(stats1, stats1_shrink, n1);

      	//--------------------------------------------------------//
        PUTS("\t Step 4 : mise en correspondance");
		kppv_routine(stats0, stats1_shrink, n0, n1_shrink, K);

        //--------------------------------------------------------//
        PUTS("\t Step 5 : recalage");
        rigid_registration(stats0, stats1_shrink, n0, n1_shrink, &theta, &tx, &ty);
        fdisp(theta);
        fdisp(tx);
        fdisp(ty);
        // saveFrame_ui32matrix(path_frame, ballon->SH32, i0, i1, j0, j1);


        PUTS("\t Step 6 : Seuillage très haut");
        copy_ui8matrix_ui8matrix(ballon->I1, i0, i1, j0, j1, ballon->SM); 
        threshold_high(ballon->SM, i0, i1, j0, j1, 60);
        convert_ui8matrix_ui32matrix(ballon->SM, i0, i1, j0, j1, ballon->SM32);
        saveFrame_ui32matrix(path_frame, ballon->SM32, i0, i1, j0, j1);

     	//--------------------------------------------------------//
        PUTS("\t Step 7 : ECC/ACC");
        n3 = CCL_LSL(ballon->SM32, i0, i1, j0, j1);
        idisp(n3);
        extract_features(ballon->SM32, i0, i1, j0, j1, stats3, n3);
        // printStats(stats3, n3);
            
      	//--------------------------------------------------------//
        PUTS("\t Step 8 : Filter speed");
        filter_surface(stats3, n3, ballon->SM32, surface_min, surface_max);
        int n3_shrink = shrink_stats(stats3, stats3_shrink, n3);


        PUTS("\t Step 4 : mise en correspondance");
		kppv_routine(stats2, stats3_shrink, n2, n3_shrink, K);

        PUTS("\t Step9 : recalage");
        motion_extraction(stats2, stats3_shrink, n2, theta, tx, ty);


        PUTS("\t Step 10: Tracking");
        Tracking(stats2, stats3_shrink, tracks, n2, n3_shrink, frame, &last, &offset);


        PUTS("[DEBUG] Saving frames");
        // sprintf(dest, "%s/%d.pgm", debug, frame);
        // disp(path_frame);
        // saveFrame_ui32matrix(path_frame, ballon->SH32, i0, i1, j0, j1);

        PUTS("\t [DEBUG] Saving stats");
        // saveStats(path_stats_0, stats0, n0);
        // saveStats(path_stats_0, stats1_shrink, n1_shrink);
        printStats(stats0, n0);
        printStats(stats1_shrink, n1_shrink);
        saveAssoConflicts(path_debug, frame-1, conflicts, nearest, distances, n0, n1_shrink, stats0, stats1_shrink); 
        printStats(stats2, n2);
        printStats(stats3_shrink, n3_shrink);
        // saveAssoConflicts(path_debug, frame-1, conflicts, nearest, distances, n2, n3_shrink, stats, stats1_shrink); 

     
        // saveMotionExtraction(path_extraction, stats0, stats1_shrink, n0, theta, tx, ty, frame-1);
        // saveError(path_error, stats0, n0);

      	//--------------------------------------------------------//
        PUTS("\t TMP");
        // cpt += TrackStars(stats0, stats1_shrink, tracks_stars, n0, n1_shrink, frame, &last_stars, &offset_stars);

      	//--------------------------------------------------------//
        // SWAP_UI8(ballon->I0, ballon->I1);
        SWAP_STATS(stats0, stats1_shrink, n1_shrink);
        SWAP_STATS(stats2, stats3_shrink, n3_shrink);
        n0 = n1_shrink;
        n2 = n3_shrink;
        frame++;
    }
    

    // printf("cpt   %d\n", cpt);
    saveTracks(path_tracks, tracks, last);


    // ----------
    // -- free --
    // ----------

    freeBallon(ballon, i0, i1, j0, j1, b);
    CCL_LSL_free(i0, i1, j0, j1);
	kppv_free(0, 50, 0, 50);
}
*/

//chaine de traitement qui prend des images
// ======================================
void meteor_ballon_hyst_frame(int argc, char** argv)
// ======================================
{

    // Parsing Arguments
    int start        = find_int_arg  (argc, argv, "-start_frame", 1 );
    int end          = find_int_arg  (argc, argv, "-end_frame", 1000);
    int light_min    = find_int_arg  (argc, argv, "-light_min",  60 ); // a definir
    int light_max    = find_int_arg  (argc, argv, "-light_max",  85 ); // a definir
    int surface_min  = find_int_arg  (argc, argv, "-surface_min",  9); // a definir
    int surface_max  = find_int_arg  (argc, argv, "-surface_max",1000); // a definir
    char* src_path   = find_char_arg (argc, argv, "-input",      NULL);

    // sequence
    char *filename;
    char src[100];
	double theta, tx, ty;
    int frame = start;

    // CC
	MeteorROI stats0[SIZE_MAX_METEORROI];
	MeteorROI stats1[SIZE_MAX_METEORROI];
	MeteorROI stats_shrink[SIZE_MAX_METEORROI];
    Track tracks[1000];

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
    if (&src_path != slash) slash++;
    filename = strndup(slash, next-slash);


  	create_debug_dir (filename, light_min , light_max, -1);
	create_frames_dir(filename, light_min , light_max, -1);
    
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
        sprintf(src, "%s%04d.pgm", src_path, frame); 
        disp(src);
        MLoadPGM_ui8matrix(src, i0, i1, j0, j1, ballon->I0);
		printf("[Frame] %-4d\n", frame-1);

	    create_debug_files (frame);
	    create_frames_files(frame);
        
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
        VERBOSE(saveStats(path_stats_0, stats1, n1); ); 

     	//--------------------------------------------------------//
        PUTS("\t Step 3 : seuillage hysteresis && filtrage surfacique"); 
        merge_HI_CCL_v2(ballon->SH32, ballon->SM32, i0, i1, j0, j1, stats1, n1, surface_min, surface_max );
        int n_shrink = shrink_stats(stats1, stats_shrink, n1);

      	//--------------------------------------------------------//
        PUTS("\t Step 4 : mise en correspondance");
		kppv_routine(stats0, stats_shrink, n0, n_shrink, K);

      	//--------------------------------------------------------//
        PUTS("\t Step 5 : recalage");
        motion(stats0, stats_shrink, n0, n_shrink, &theta, &tx, &ty);

        PUTS("\t Step 6: Tracking");
        Tracking(stats0, stats_shrink, tracks, n0, n_shrink, frame, &last, &offset, theta, tx, ty);
        
        // VERBOSE(printStats(stats0, n0); ); 
        // VERBOSE(printStats(stats_shrink, n_shrink); ); 

        //--------------------------------------------------------//
        PUTS("\t [DEBUG] Saving frames");
        // saveFrame_ui32matrix(path_frame, ballon->SH32, i0, i1, j0, j1);
    	// saveFrame_quad_hysteresis(path_frame, ballon->I0, ballon->SH32, ballon->SB32, ballon->SH32, i0, i1, j0, j1);

        PUTS("\t [DEBUG] Saving stats");
        // saveStats(path_stats_0, stats0, n0);
        // saveStats(path_stats_0, stats_shrink, n_shrink);
        // saveAssoConflicts(path_debug, frame-1, conflicts, nearest, distances, n0, n_shrink, stats0, stats_shrink); 
        // saveMotion(path_motion, theta, tx, ty, frame-1);
        // saveMotionExtraction(path_extraction, stats0, stats_shrink, n0, theta, tx, ty, frame-1);
        // saveError(path_error, stats0, n0);

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
        fprintf(stderr, "usage: %s %s [options] <input_file>\n", argv[0], argv[1]);
        fprintf(stderr, "  -start_frame  : Image de départ dans la séquence\n");
        fprintf(stderr, "  -end_frame    : Dernière image de la séquence\n");
        fprintf(stderr, "  -skip_frames  : Nombre d'images à sauter\n");
        fprintf(stderr, "  -light        : Seuil filtrage lumineux\n");
        fprintf(stderr, "  -light_min    : Seuil filtrage lumineux\n");
        fprintf(stderr, "  -light_max    : Seuil filtrage lumineux\n");
        fprintf(stderr, "  -surface_min  : Surface max des CC en pixels\n");
        fprintf(stderr, "  -surface_max  : Surface min des CC en pixels\n");
        fprintf(stderr, "  -edt          : Seuil EDT\n");
        fprintf(stderr, "  -validation   : Fichier contenant la vérité terrain de la séquence\n");
        exit(1);
    }

    // Parsing Arguments
    int start        = find_int_arg  (argc, argv, "-start_frame", 0 );
    int end          = find_int_arg  (argc, argv, "-end_frame", 1000);
    int skip         = find_int_arg  (argc, argv, "-skip_frames", 0 );
    int light_min    = find_int_arg  (argc, argv, "-light_min",  60 ); // a definir
    int light_max    = find_int_arg  (argc, argv, "-light_max",  85 ); // a definir
    int surface_min  = find_int_arg  (argc, argv, "-surface_min",  9); // a definir
    int surface_max  = find_int_arg  (argc, argv, "-surface_max",100); // a definir
    char* src_path   = find_char_arg (argc, argv, "-input",      NULL);
    char* validation = find_char_arg (argc, argv, "-validation", NULL);

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

    // int  offset_stars =  0;
    // int  last_stars   = -1;
    // int cpt = 0;


	int n0 = 0;
	int n1 = 0;

    // image
    int b = 1;                  
    int i0, i1, j0, j1;

    

    //path management
    char *path;
    split_path_file(&path, &filename, src_path);

  	create_debug_dir (filename, light_min , light_max, -1);
	create_frames_dir(filename, light_min , light_max, -1);
    
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


    PUTS("INIT");
    initBallon(ballon, i0, i1, j0, j1, b);
    PUTS("INIT ballon");

	kppv_init(0, SIZE_MAX_KPPV, 0, SIZE_MAX_KPPV);
    PUTS("INIT kppv");
	init_MeteorROI(stats0, SIZE_MAX_METEORROI);
	init_MeteorROI(stats1, SIZE_MAX_METEORROI);
    PUTS("INIT Track");
    init_Track(tracks, SIZE_MAX_TRACKS);
    init_Track(tracks_stars, SIZE_MAX_TRACKS);
    PUTS("INIT CCL");
    CCL_LSL_init(i0, i1, j0, j1);

    initTabBB();

    if (validation) 
        Validation(validation,tracks, 1000, "./debug/");


    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//
    PUTS("LOOP");

	if(!Video_nextFrame(video,ballon->I0)) { 
        exit(1);
    }

    while(Video_nextFrame(video,ballon->I1)) {
        frame = video->frame_current - 1;
		printf("[Frame] %-4d\n", frame);

	    create_debug_files (frame);
	    create_frames_files(frame);
	    create_videos_files(filename);

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
        idisp(n1);
        extract_features(ballon->SM32, i0, i1, j0, j1, stats1, n1);

     	//--------------------------------------------------------//
        PUTS("\t Step 3 : seuillage hysteresis && filter surface"); 
        merge_HI_CCL_v2(ballon->SH32, ballon->SM32, i0, i1, j0, j1, stats1, n1, surface_min, surface_max); 
        int n_shrink = shrink_stats(stats1, stats_shrink, n1);

      	//--------------------------------------------------------//
        PUTS("\t Step 4 : mise en correspondance");
		kppv_routine(stats0, stats_shrink, n0, n_shrink, K);

      	//--------------------------------------------------------//
        PUTS("\t Step 5 : recalage");
        motion(stats0, stats_shrink, n0, n_shrink, &theta, &tx, &ty);

        PUTS("\t Step 6: Tracking");
        Tracking(stats0, stats_shrink, tracks, n0, n_shrink, frame, &last, &offset, theta, tx, ty);
        
        //--------------------------------------------------------//
        PUTS("\t [DEBUG] Saving frames");
        // saveFrame_ui32matrix(path_frame, ballon->SH32, i0, i1, j0, j1);
    	// saveFrame_quad_hysteresis(path_frame, ballon->I0, ballon->SH32, ballon->SB32, ballon->SH32, i0, i1, j0, j1);

        PUTS("\t [DEBUG] Saving stats");
        // saveStats(path_stats_0, stats0, n0);
        // saveStats(path_stats_0, stats_shrink, n_shrink);
        saveAssoConflicts(path_debug, frame-1, conflicts, nearest, distances, n0, n_shrink, stats0, stats_shrink); 
        // saveMotion(path_motion, theta, tx, ty, frame-1);
        // saveMotionExtraction(path_extraction, stats0, stats_shrink, n0, theta, tx, ty, frame-1);
        // saveError(path_error, stats0, n0);
        // disp(path_video_tracking);
        // saveFrame_tracking(path_frame, ballon->I0, tracks, last, i0, i1, j0, j1);
        // saveVideoFrame_tracking(path_video_tracking, ballon->I0, tracks, last, i0, i1, j0, j1);

      	//--------------------------------------------------------//
        PUTS("\t TMP");
        // cpt += TrackStars(stats0, stats_shrink, tracks_stars, n0, n_shrink, frame, &last_stars, &offset_stars);

      	//--------------------------------------------------------//
        SWAP_UI8(ballon->I0, ballon->I1);
        SWAP_STATS(stats0, stats_shrink, n_shrink);
        n0 = n_shrink;
    }
    
    saveTabBB(path_bounding_box, tabBB, NB_FRAMES);

    // printf("cpt   %d\n", cpt);
    disp(path_tracks);
    saveTracks(path_tracks, tracks, last);

    if (validation) 
        Validation_final();


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

int main(int argc, char** argv)
{
    // meteor_ballon_hyst_frame(argc, argv); // 
    meteor_ballon_hyst(argc, argv); // 

    return 0;
}





