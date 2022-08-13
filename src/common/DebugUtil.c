/**
 * Copyright (c) 2017-2018, Arthur Hennequin, LIP6, UPMC, CNRS
 * Copyright (c) 2020-2020, Lionel Lacassagne, LIP6 Sorbonne University, CNRS
 * Copyright (c) 2020-2021, MILLET Maxime, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#include <assert.h>
#include <ffmpeg-io/reader.h>
#include <ffmpeg-io/writer.h>
#include <math.h>
#include <nrc2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "Args.h"
#include "CCL.h"
#include "DebugUtil.h"
#include "Features.h"
#include "Threshold.h"
#include "Tracking.h"
#include "Video.h"
#include "macro_debug.h"
#include "tools_visu.h"

#define SIZE_BUF 20

char g_path_stats_0[250], g_path_stats_1[250];
char g_path_frames_binary[250], g_path_frames_output[250];
char g_path_video_tracking[250];
char g_path_motion[200], g_path_extraction[200], g_path_error[200], g_path_tracks[200], g_path_bounding_box[200];
char g_path_debug[250];
char g_path_frames_binary_dir[200], g_path_frames_output_dir[200], g_path_stats_f[200], g_path_video_f[200];
char g_path_assoconflicts_f[150];

void printStats(ROI_t* stats, int n)
{
    int cpt = 0;
    for (int i = 1; i <= n; i++) {
        if (stats[i].S > 0) {
            cpt++;
        }
    }
    printf("Nombre de CC : %d\n", cpt);

    if (cpt == 0)
        return;

    for (int i = 1; i <= n; i++) {
        if (stats[i].S > 0)
            printf("%4d \t %4d \t %4d \t %4d \t %4d \t %3d \t %4d \t %4d \t %7.1f \t %7.1f \t %4d \t %4d \t %4d \t "
                   "%7.1lf \t %d\n",
                   stats[i].ID, stats[i].xmin, stats[i].xmax, stats[i].ymin, stats[i].ymax, stats[i].S, stats[i].Sx,
                   stats[i].Sy, stats[i].x, stats[i].y, stats[i].prev, stats[i].next, stats[i].time, stats[i].error,
                   stats[i].motion);
    }
    printf("\n");
}

void printBuffer(ROIx2_t* buffer, int n)
{
    for (int i = 0; i < n; i++) {
        if (buffer[i].stats0.ID > 0)
            printf("i = %2d \t %4d \t %4d \t %4d  \n", i, buffer[i].stats0.ID, buffer[i].stats1.time, buffer[i].frame);
    }
    printf("\n");
}

void printTabBB(BB_t** tabBB, int n) {
    for (int i = 0; i < n; i++) {
        if (tabBB[i] != NULL) {
            for (BB_t* current = tabBB[i]; current != NULL; current = current->next) {
                printf("%d %d %d %d %d %d \n", i, current->rx, current->ry, current->bb_x, current->bb_y,
                       current->track_id);
            }
        }
    }
}

void saveTabBB(const char* filename, BB_t** tabBB, track_t* tracks, int n, int track_all) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        if (tabBB[i] != NULL) {
            for (BB_t* current = tabBB[i]; current != NULL; current = current->next) {
                if (track_all || (!track_all && tracks[(current->track_id) - 1].obj_type == METEOR))
                    fprintf(f, "%d %d %d %d %d %d \n", i, current->rx, current->ry, current->bb_x, current->bb_y,
                            current->track_id);
            }
        }
    }

    fclose(f);
}

void saveErrorMoy(const char* filename, double errMoy, double eType)
{
    char path[200];
    sprintf(path, "%s/%s", g_path_assoconflicts_f, filename);
    disp(path);
    FILE* f = fopen(path, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", path);
        exit(1);
    }
    fprintf(f, "%5.2f \t %5.2f \n", errMoy, eType);

    fclose(f);
}

void print_tracks(track_t* tracks, int last)
{
    printf("%d\n", last + 1);

    if (last == -1)
        return;

    for (int i = 0; i <= last; i++)
        printf("%4d \t %6.1f \t %6.1f \t %4d \t %6.1f \t %6.1f \t %4d \t %4d \t %4d \t %4d \t %4d %4d\n",
               tracks[i].timestamp, tracks[i].begin.x, tracks[i].begin.y, tracks[i].timestamp + tracks[i].time,
               tracks[i].end.x, tracks[i].end.y, tracks[i].rx, tracks[i].ry, tracks[i].bb_x, tracks[i].bb_y,
               tracks[i].is_valid, tracks[i].obj_type);
}

void print_tracks2(FILE* f, track_t* tracks, int n)
{
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "#  track ||           Begin           ||            End            ||  Object \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");

    unsigned track_id = 0;
    for (int i = 0; i < n; i++)
        if (tracks[i].time) {
            fprintf(f, "   %5d || %7d | %6.1f | %6.1f || %7d | %6.1f | %6.1f || %s \n", tracks[i].id,
                    tracks[i].timestamp, tracks[i].begin.x, tracks[i].begin.y, tracks[i].timestamp + tracks[i].time,
                    tracks[i].end.x, tracks[i].end.y, g_obj_type_to_string_with_spaces[tracks[i].obj_type]);
            track_id++;
        }
}

void parseStats(const char* filename, ROI_t* stats, int* n)
{
    char lines[200];
    int id, xmin, xmax, ymin, ymax, s, sx, sy, prev, next;
    double x, y;
    float32 dx, dy, error;
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "(EE) cannot open file '%s'\n", filename);
        return;
    }

    // pour l'instant, n représente l'id max des stas mais c'est à changer
    fgets(lines, 100, file);
    sscanf(lines, "%d", n);

    while (fgets(lines, 200, file)) {
        sscanf(lines, "%d %d %d %d %d %d %d %d %lf %lf %f %f %f %d %d", &id, &xmin, &xmax, &ymin, &ymax, &s, &sx, &sy,
               &x, &y, &dx, &dy, &error, &prev, &next);
        stats[id].ID = id;
        stats[id].xmin = xmin;
        stats[id].xmax = xmax;
        stats[id].ymin = ymin;
        stats[id].ymax = ymax;
        stats[id].S = s;
        stats[id].Sx = sx;
        stats[id].Sy = sy;
        stats[id].x = x;
        stats[id].y = y;
        stats[id].dx = dx;
        stats[id].dy = dy;
        stats[id].error = error;
        stats[id].prev = prev;
        stats[id].next = next;
    }
    fclose(file);
}

void saveStats_file(FILE* f, ROI_t* stats, int n, track_t* tracks)
{
    int cpt = 0;
    for (int i = 1; i <= n; i++) {
        // idisp(i);
        if (stats[i].S != 0)
            cpt++;
    }
    fprintf(f, "# Regions of interest (ROI) [%d]: \n", cpt);
    if (cpt) {
        fprintf(f, "# ------||----------------||---------------------------||---------------------------||-------------------\n");
        fprintf(f, "#   ROI ||      track     ||        Bounding Box       ||   Surface (S in pixels)   ||      Center       \n");
        fprintf(f, "# ------||----------------||---------------------------||---------------------------||-------------------\n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------\n");
        fprintf(f, "#    ID ||   ID |    Type || xmin | xmax | ymin | ymax ||   S |       Sx |       Sy ||       x |       y \n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------\n");
    }

    for (int i = 1; i <= n; i++) {
        if (stats[i].S != 0) {
            fprintf(f, "   %4d || %4d | %s || %4d | %4d | %4d | %4d || %3d | %8d | %8d || %7.1f | %7.1f  \n",
                    stats[i].ID, stats[i].track_id,
                    g_obj_type_to_string_with_spaces[tracks[stats[i].track_id - 1].obj_type], stats[i].xmin,
                    stats[i].xmax, stats[i].ymin, stats[i].ymax, stats[i].S, stats[i].Sx, stats[i].Sy, stats[i].x,
                    stats[i].y);
        }
    }
}

void saveStats(const char* filename, ROI_t* stats, int n, track_t* tracks)
{
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }
    saveStats_file(f, stats, n, tracks);
    fclose(f);
}

void save_tracks(const char* filename, track_t* tracks, int n)
{
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    int cpt = 0;
    for (int i = 0; i <= n; i++) {
        if (tracks[i].time)
            cpt++;
    }

    fprintf(f, "%d\n", cpt);

    if (cpt != 0) {
        for (int i = 0; i <= n; i++) {
            if (tracks[i].time) {
                fprintf(f, "%4d \t %6.1f \t %6.1f \t %4d \t %6.1f \t %6.1f \t %4d \t %4d \t %4d\n", tracks[i].timestamp,
                        tracks[i].begin.x, tracks[i].begin.y, tracks[i].timestamp + tracks[i].time, tracks[i].end.x,
                        tracks[i].end.y, tracks[i].bb_x, tracks[i].bb_y, tracks[i].obj_type);
            }
        }
    }
    fclose(f);
}

void saveBoundingBox(const char* filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame)
{
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }
    fprintf(f, "%4d \t %4d \t %4d \t %4d \t %4d \n", frame, rx, ry, bb_x, bb_y);
    fclose(f);
}

void parse_tracks(const char* filename, track_t* tracks, int* n)
{
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "(EE) Can't open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    int tid, t0, t1;
    float32 x0, x1, y0, y1;
    // int bb_x, bb_y;
    // int obj_type;
    char obj_type_str[1024];

    *n = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        if (line[0] != '#') {
            sscanf(line, "%d || %d | %f | %f || %d | %f | %f || %s ", &tid, &t0, &x0, &y0, &t1, &x1, &y1, obj_type_str);

            tracks[*n].id = tid;
            tracks[*n].timestamp = t0;
            tracks[*n].time = t1 - t0;
            tracks[*n].state = 0;
            tracks[*n].begin.x = x0;
            tracks[*n].begin.y = y0;
            tracks[*n].end.x = x1;
            tracks[*n].end.y = y1;
            // tracks[*n].bb_x   = bb_x;
            // tracks[*n].bb_y   = bb_y;
            tracks[*n].obj_type = tracking_string_to_obj_type((const char*)obj_type_str);
            (*n)++;
        }
    }

    fclose(fp);
    if (line)
        free(line);
}

void saveMotion(const char* filename, double theta, double tx, double ty, int frame)
{
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    fprintf(f, "%d - %d\n", frame, frame + 1);
    fprintf(f, "%6.7f \t %6.4f \t %6.4f \n", theta, tx, ty);
    fprintf(f, "---------------------------------------------------------------\n");
    fclose(f);
}

void saveError(const char* filename, ROI_t* stats, int n)
{
    double S = 0;
    int cpt = 0;
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = 1; i <= n; i++) {
        if (stats[i].S > 0) {
            S += stats[i].error;
            cpt++;
        }
    }

    fprintf(f, "%.2f\n", S / cpt);
    fclose(f);
}

void saveAsso(const char* filename, uint32** Nearest, float32** distances, int nc0, ROI_t* stats)
{
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for (int i = 1; i <= nc0; i++) {
        if (stats[i].S != 0 && stats[i].next)
            cpt++;
    }
    if (cpt != 0) {

        fprintf(f, "%d\n", cpt);

        int j;
        for (int i = 1; i <= nc0; i++) {
            j = stats[i].next;
            if (j == 0) {
                if (stats[i].S > 0)
                    fprintf(f, "%4d \t ->   pas d'association\n", i);
            } else {
                fprintf(f, "%4d \t -> %4d \t  : distance = %10.2f \t ; %4d-voisin\n", i, j, distances[i][j],
                        Nearest[i][j]);
            }
        }
    }
    fclose(f);
}

void saveAsso_VT(const char* filename, int nc0, ROI_t* stats, int frame)
{
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }
    fprintf(f, "%05d_%05d\n", frame, frame + 1);

    int j;
    for (int i = 1; i <= nc0; i++) {
        j = stats[i].next;
        if (j == 0) {
            if (stats[i].S > 0)
                fprintf(f, "%4d \t ->   pas d'association\n", i);
        } else {
            fprintf(f, "%4d \t -> %4d \n", i, j);
        }
    }
    fprintf(f, "-------------------------------------------------------------------------------------------------------"
               "-----\n");
    fclose(f);
}

void saveConflicts(const char* filename, uint32* conflicts, uint32** Nearest, float32** distances, int n_asso,
                   int n_conflict)
{
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for (int i = 1; i <= n_conflict; i++) {
        if (conflicts[i] != 1 && conflicts[i] != 0)
            cpt++;
    }
    if (cpt != 0) {

        fprintf(f, "%d\n", cpt);

        for (int j = 1; j <= n_conflict; j++) {
            if (conflicts[j] != 1 && conflicts[j] != 0) {
                fprintf(f, "conflit CC = %4d : ", j);
                for (int i = 1; i <= n_asso; i++) {
                    if (Nearest[i][j] == 1) {
                        fprintf(f, "%4d\t", i);
                    }
                }
                fprintf(f, "\n");
            }
        }
    }
    fclose(f);
}

void saveAssoConflicts(const char* path, int frame, uint32* conflicts, uint32** Nearest, float32** distances,
                       int n_asso, int n_conflict, ROI_t* stats0, ROI_t* stats1, track_t* tracks, int n_tracks)
{
    assert(frame >= 0);

    char filename[1024];

    sprintf(filename, "%s/%05d_%05d.txt", path, frame, frame + 1);

    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    // stats
    fprintf(f, "# Frame n°%05d (cur)\n", frame);
    saveStats_file(f, stats0, n_asso, tracks);
    fprintf(f, "#\n# Frame n°%05d (next)\n", frame + 1);
    saveStats_file(f, stats1, n_conflict, tracks);
    fprintf(f, "#\n");

    // Asso
    int cpt = 0;
    for (int i = 1; i <= n_asso; i++) {
        if (stats0[i].next != 0)
            cpt++;
    }
    fprintf(f, "# Associations [%d]:\n", cpt);
    int j;

    if (cpt) {
        double errMoy = features_error_moy(stats0, n_asso);
        double eType = features_ecart_type(stats0, n_asso, errMoy);
        fprintf(f, "# * mean error    = %.3f\n", errMoy);
        fprintf(f, "# * std deviation = %.3f\n", eType);

        fprintf(f, "# ------------||---------------||------------------------\n");
        fprintf(f, "#    ROI ID   ||    Distance   ||          Error         \n");
        fprintf(f, "# ------------||---------------||------------------------\n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------\n");
        fprintf(f, "#  cur | next || pixels | k-nn ||    dx |    dy |      e \n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------\n");
    }

    for (int i = 1; i <= n_asso; i++) {
        if (stats0[i].S == 0)
            continue;

        j = stats0[i].next;
        if (j != 0) {
            float32 dx = stats0[i].dx;
            float32 dy = stats0[i].dy;
            fprintf(f, "  %4d | %4d || %6.2f | %4d || %5.1f | %5.1f | %6.3f \n", i, j, distances[i][j], Nearest[i][j],
                    dx, dy, stats0[i].error);
        }
    }

    fprintf(f, "#\n");
    fprintf(f, "# tracks [%d]:\n", n_tracks);
    if (n_tracks)
        print_tracks2(f, tracks, n_tracks);

    // // Conflicts
    // cpt = 0;
    // for(int i = 1; i<= n_conflict; i++){
    //     if(conflicts[i] != 1 && conflicts[i] != 0)
    //         cpt++;
    // }

    // fprintf(f, "Conflicts\n%d\n", cpt);

    // for(int j = 1; j <= n_conflict; j++){
    //     if (conflicts[j] != 1 && conflicts[j] != 0){
    //         fprintf(f, "conflit CC = %4d : ", j);
    //         for(int i = 1 ; i <= n_asso; i++){
    //             if (Nearest[i][j] == 1 ){
    //                 fprintf(f, "%4d\t", i);
    //             }
    //         }
    //         fprintf(f, "\n");
    //     }
    // }
    fclose(f);
}

void saveMotionExtraction(char* filename, ROI_t* stats0, ROI_t* stats1, int nc0, double theta, double tx,
                          double ty, int frame)
{
    // Version DEBUG : il faut implémenter une version pour le main
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "motion : cannot open file\n");
        return;
    }

    double errMoy = features_error_moy(stats0, nc0);
    double eType = features_ecart_type(stats0, nc0, errMoy);

    for (int i = 1; i <= nc0; i++) {
        float32 e = stats0[i].error;
        // si mouvement detecté
        if (fabs(e - errMoy) > 1.5 * eType) {
            fprintf(f, "%d - %d\n", frame, frame + 1);
            fprintf(f,
                    "CC en mouvement: %2d \t dx:%.3f \t dy: %.3f \t xmin: %3d \t xmax: %3d \t ymin: %3d \t ymax: %3d\n",
                    stats0[i].ID, stats0[i].dx, stats0[i].dy, stats0[i].xmin, stats0[i].xmax, stats0[i].ymin,
                    stats0[i].ymax);
            fprintf(f, "---------------------------------------------------------------\n");
        }
    }
    fclose(f);
}

void filter_speed_binarize(uint32** in, int i0, int i1, int j0, int j1, uint8** out, ROI_t* stats) {
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (stats[in[i][j]].S != 0) {
                out[i][j] = 0xFF;
            } else {
                out[i][j] = 0;
            }
        }
    }
}

void plot_bounding_box(rgb8** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8 color) {
    for (int b = 0; b < border; b++) {
        ymin++;
        ymax--;
        xmin++;
        xmax--;
        for (int i = ymin; i <= ymax; i++) {
            img[i][xmin] = color;
            img[i][xmax] = color;
        }
        for (int j = xmin; j <= xmax; j++) {
            img[ymin][j] = color;
            img[ymax][j] = color;
        }
    }
}

rgb8** load_image_color(const char* filename, long* i0, long* i1, long* j0, long* j1)
{
    VERBOSE(printf("%s\n", filename););
    ffmpeg_handle reader;
    ffmpeg_init(&reader);
    ffmpeg_options options;
    ffmpeg_options_init(&options);

    options.debug = 1;

    if (!ffmpeg_probe(&reader, filename, &options)) {
        fprintf(stderr, "(EE) Error in load_image_color: %s (%d)\n", ffmpeg_error2str(reader.error), reader.error);
        exit(0);
    }
    reader.output.pixfmt = ffmpeg_str2pixfmt("rgb24");
    *i0 = 0;
    *j0 = 0;
    *i1 = reader.input.height - 1;
    *j1 = reader.input.width - 1;
    VERBOSE(printf("i1=%ld j1=%ld\n", *i1, *j1););
    rgb8** img = rgb8matrix(*i0, *i1, *j0, *j1);

    ffmpeg_start_reader(&reader, filename, NULL);
    ffmpeg_read2d(&reader, (uint8_t**)img);
    ffmpeg_stop_reader(&reader);
    return img;
}

void saveFrame_threshold(const char* filename, uint8** I0, uint8** I1, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, h - 1, 0, 2 * w - 1);
    if (img == NULL)
        return;

    // (0,0) : video
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage luminosité
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j + w].r = I1[i][j];
            img[i][j + w].g = I1[i][j];
            img[i][j + w].b = I1[i][j];
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        char message[256] = "ouverture du fichier %s impossible dans saveVideoFrame_quad\n";
        nrerror(message);
    }

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        WritePNMrow((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h - 1, 0, 2 * w - 1);
}

void saveFrame_ui32matrix(const char* filename, uint32** I, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, h - 1, 0, w - 1);
    if (img == NULL)
        return;

    // (1,1) : filtrage surface
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = (I[i][j] == 0) ? 0 : 255;
            img[i][j].g = (I[i][j] == 0) ? 0 : 255;
            img[i][j].b = (I[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        char message[256] = "ouverture du fichier %s impossible dans saveVideoFrame_quad\n";
        nrerror(message);
    }

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        WritePNMrow((uint8*)img[i], w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h - 1, 0, w - 1);
}

void saveFrame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0, int j1) {
    rgb8 green;
    green.g = 255;
    green.b = 000;
    green.r = 000;

    rgb8 red;
    red.g = 000;
    red.b = 000;
    red.r = 255;

    rgb8 blue;
    blue.g = 000;
    blue.b = 255;
    blue.r = 000;

    rgb8 orange;
    orange.r = 255;
    orange.g = 165;
    orange.b = 000;

    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, h - 1, 0, w - 1);
    if (img == NULL)
        return;

    // (1,1) : filtrage surface
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    for (int i = 0; i < tracks_nb + 1; i++) {
        idisp(tracks[i].time);
        idisp(tracks[i].state);
        if (tracks[i].state != TRACK_LOST && tracks[i].time >= 2 && tracks[i].state != TRACK_EXTRAPOLATED &&
            tracks[i].state != TRACK_FINISHED) {
            int ymin = clamp(tracks[i].bb_y - tracks[i].ry, 1, i1 - 1);
            int ymax = clamp(tracks[i].bb_y + tracks[i].ry, 1, i1 - 1);
            int xmin = clamp(tracks[i].bb_x - tracks[i].rx, 1, j1 - 1);
            int xmax = clamp(tracks[i].bb_x + tracks[i].rx, 1, j1 - 1);
            idisp(xmin);
            idisp(xmax);
            idisp(ymin);
            idisp(ymax);
            if (tracks[i].time >= 2) {
                // if(tracks[i].is_valid) {

                // if(tracks[i].state != TRACK_EXTRAPOLATED)
                plot_bounding_box(img, ymin, ymax, xmin, xmax, 1, green);
                // else plot_bounding_box(img, ymin,ymax,xmin,xmax, 2, blue);
            }
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        char message[256] = "ouverture du fichier %s impossible dans saveVideoFrame_quad\n";
        nrerror(message);
    }

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        WritePNMrow((uint8*)img[i], w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h - 1, 0, w - 1);
}

void saveVideoFrame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0,
                             int j1) {
    rgb8 green;
    green.g = 255;
    green.b = 000;
    green.r = 000;

    rgb8 red;
    red.g = 000;
    red.b = 000;
    red.r = 255;

    rgb8 blue;
    blue.g = 000;
    blue.b = 255;
    blue.r = 000;

    rgb8 orange;
    orange.r = 255;
    orange.g = 165;
    orange.b = 000;

    static ffmpeg_handle writer;
    if (writer.pipe == NULL) {
        ffmpeg_init(&writer);
        writer.input.width = j1 - j0 + 1;
        writer.input.height = i1 - i0 + 1;
        writer.input.pixfmt = ffmpeg_str2pixfmt("rgb24");
        if (!ffmpeg_start_writer(&writer, filename, NULL))
            return;
    }
    rgb8** img = rgb8matrix(0, i1, 0, j1);
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    for (int i = 0; i < tracks_nb + 1; i++) {
        idisp(tracks[i].time);
        idisp(tracks[i].state);
        if (tracks[i].state != TRACK_LOST && tracks[i].time >= 3 && tracks[i].state != TRACK_EXTRAPOLATED &&
            tracks[i].state != TRACK_FINISHED) {
            int ymin = clamp(tracks[i].bb_y - tracks[i].ry - 5, 1, i1 - 1);
            int ymax = clamp(tracks[i].bb_y + tracks[i].ry + 5, 1, i1 - 1);
            int xmin = clamp(tracks[i].bb_x - tracks[i].rx - 5, 1, j1 - 1);
            int xmax = clamp(tracks[i].bb_x + tracks[i].rx + 5, 1, j1 - 1);
            idisp(xmin);
            idisp(xmax);
            idisp(ymin);
            idisp(ymax);
            if (tracks[i].time >= 3) {
                // if(tracks[i].is_valid) {

                // if(tracks[i].state != TRACK_EXTRAPOLATED)
                plot_bounding_box(img, ymin, ymax, xmin, xmax, 2, green);
                // else plot_bounding_box(img, ymin,ymax,xmin,xmax, 2, blue);
            }
        }
    }
    ffmpeg_write2d(&writer, (uint8_t**)img);
    free_rgb8matrix(img, 0, i1, 0, j1);
}

void saveFrame_ui8matrix(const char* filename, uint8** I, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, h - 1, 0, w - 1);
    if (img == NULL)
        return;

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        char message[256] = "ouverture du fichier %s impossible dans saveVideoFrame_quad\n";
        nrerror(message);
    }

    /* enregistrement de l'image au format rpgm */
    sprintf(buffer, "P6\n%d %d\n255\n", (int)(w - 1), (int)(h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= h - 1; i++)
        WritePNMrow((uint8*)img[i], w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h - 1, 0, w - 1);
}

void HsvToRgb(rgb8* pixel, uint8 h, uint8 s, uint8 v) {
    unsigned char region, remainder, p, q, t;

    if (s == 0) {
        pixel->r = v;
        pixel->g = v;
        pixel->b = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
    case 0:
        pixel->r = v;
        pixel->g = t;
        pixel->b = p;
        break;
    case 1:
        pixel->r = q;
        pixel->g = v;
        pixel->b = p;
        break;
    case 2:
        pixel->r = p;
        pixel->g = v;
        pixel->b = t;
        break;
    case 3:
        pixel->r = p;
        pixel->g = q;
        pixel->b = v;
        break;
    case 4:
        pixel->r = t;
        pixel->g = p;
        pixel->b = v;
        break;
    default:
        pixel->r = v;
        pixel->g = p;
        pixel->b = q;
        break;
    }
}

void saveFrame_quad(const char* filename, uint8** I0, uint8** I1, uint32** I2, uint32** I3, int nbLabel,
                    ROI_t* stats, int i0, int i1, int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, 2 * h - 1, 0, 2 * w - 1);
    if (img == NULL)
        return;

    // (0,0) : video
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage luminosité
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j + w].r = I1[i][j];
            img[i][j + w].g = I1[i][j];
            img[i][j + w].b = I1[i][j];
        }
    }
    // (1,0) : CCL
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (I2[i][j] != 0) {
                HsvToRgb((img[i + h]) + j, (uint8)(I2[i][j] * 360 / 5), 255, 255);
            } else {
                HsvToRgb((img[i + h]) + j, 0, 0, 0);
            }
        }
    }
    // Carrés vert sur les CC
    // for (int k=1 ; k<=nbLabel ; k++) {
    //     if (stats[k].S > 3) {
    //         rgb8 color;
    //         //HsvToRgb(&color, k*255 / nbLabel, 255, 255);
    //         color.r = 0;
    //         color.g = 255;
    //         color.b = 0;
    //         int ymin = min(stats[k].ymin + h - 10, h*2-1);
    //         int ymax = min(stats[k].ymax + h + 10, h*2-1);
    //         int xmin = min(stats[k].xmin     - 10, w  -1);
    //         int xmax = min(stats[k].xmax     + 10, w  -1);

    //         for (int b=0; b<5 ; b++) {
    //             ymin++; ymax--;xmin++;xmax--;
    //             for (int i=ymin ; i<=ymax ; i++) {
    //                 img[i][xmin] = color;
    //                 img[i][xmax] = color;
    //             }
    //             for (int j=xmin ; j<=xmax ; j++) {
    //                 img[ymin][j] = color;
    //                 img[ymax][j] = color;
    //             }
    //         }
    //     }
    // }

    // (1,1) : filtrage surface
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i + h][j + w].r = (I3[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].g = (I3[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].b = (I3[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        char message[256] = "ouverture du fichier %s impossible dans saveVideoFrame_quad\n";
        nrerror(message);
    }

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= 2 * h - 1; i++)
        WritePNMrow((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, 2 * h - 1, 0, 2 * w - 1);
}

void saveFrame_quad_hysteresis(const char* filename, uint8** I0, uint32** SH, uint32** SB, uint32** Y, int i0, int i1,
                               int j0, int j1) {
    int w = (j1 - j0 + 1);
    int h = (i1 - i0 + 1);

    char buffer[80];

    FILE* file;

    rgb8** img = rgb8matrix(0, 2 * h - 1, 0, 2 * w - 1);
    if (img == NULL)
        return;

    // (0,0) : video
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage low
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (SB[i][j] != 0) {
                HsvToRgb((img[i]) + j + w, (uint8)(SB[i][j] * 360 / 5), 255, 255);
            } else {
                HsvToRgb((img[i]) + j + w, 0, 0, 0);
            }
        }
    }

    // (0,1) : seuillage high
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            if (SH[i][j] != 0) {
                HsvToRgb((img[i + h]) + j, (uint8)(SH[i][j] * 360 / 5), 255, 255);
            } else {
                HsvToRgb((img[i + h]) + j, 0, 0, 0);
            }
        }
    }

    // (0,0) : out
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            img[i + h][j + w].r = (Y[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].g = (Y[i][j] == 0) ? 0 : 255;
            img[i + h][j + w].b = (Y[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL) {
        char message[256] = "ouverture du fichier %s impossible dans saveVideoFrame_quad\n";
        nrerror(message);
    }

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer, "P6\n%d %d\n255\n", (int)(2 * w - 1), (int)(2 * h - 1));
    fwrite(buffer, strlen(buffer), 1, file);
    for (int i = 0; i <= 2 * h - 1; i++)
        WritePNMrow((uint8*)img[i], 2 * w - 1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, 2 * h - 1, 0, 2 * w - 1);
}

void saveMax(const char* filename, uint8** I, int i0, int i1, int j0, int j1) {
    uint8 m;
    uint8* res = ui8vector(i0, i1);

    zero_ui8vector(res, i0, i1);

    for (int i = i0; i <= i1; i++) {
        m = 0;
        for (int j = j0; j <= j1; j++) {
            m = max(m, I[i][j]);
        }
        res[i] = m;
    }

    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        printf("error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = i1; i >= i0; i--) {
        fprintf(f, "%4d\t%4d\n", i, res[i]);
    }
    fclose(f);
}

void split_path_file(char** p, char** f, char* pf) {
    assert(pf != NULL);
    assert(p != NULL);
    assert(f != NULL);

    char *slash = pf, *next;
    while ((next = strpbrk(slash + 1, "\\/")))
        slash = next;

    if (pf != slash)
        slash++;
    *p = strndup(pf, slash - pf);
    next = strpbrk(slash + 1, ".");
    *f = strndup(slash, next - slash);
}

void get_data_from_tracks_path(char* path, int* light_min, int* light_max, char** filename) {
    disp(path);
    char *res, *tmp, *min, *max;
    res = strstr(path, "SB_");
    res += 3;
    tmp = strchr(res, '_');
    min = strndup(res, tmp - res);
    *light_min = atoi(min);

    res = tmp + 4;
    tmp = strchr(res, '/');
    max = strndup(res, tmp - res);
    *light_max = atoi(max);

    res = tmp + 1;
    tmp = strchr(res, '/');
    // *filename = strndup(res, tmp - res);

    tmp = strchr(res, '_');
    res = tmp + 1;
    tmp = strchr(res, '.');

    *filename = strndup(res, tmp - res);

    idisp(*light_min);
    idisp(*light_max);
    disp(*filename);
    free(min);
    free(max);
}

void get_bouding_box_path_from_tracks_path(char* path_tracks) {
    assert(path_tracks != NULL);
    // assert(path_bounding_box != NULL);

    char *slash = path_tracks, *next;
    while ((next = strpbrk(slash + 1, "\\/")))
        slash = next;

    if (path_tracks != slash)
        slash++;
    next = strndup(path_tracks, slash - path_tracks);
    disp(next);

    // *path_bb = malloc(sizeof(strlen(next)) + strlen("/bounding_box.txt") + 1);
    sprintf(g_path_bounding_box, "%sbounding_box.txt", next);
    disp(g_path_bounding_box);
    disp(next);
    free(next);
}

void create_debug_dir(char* output_dest) {
    struct stat status = {0};

    sprintf(g_path_debug, "%s/", output_dest);

    if (stat(output_dest, &status) == -1) {
        mkdir(output_dest, 0700);
    }
}

void create_tracks_dir(char* output_dest) {
    struct stat status = {0};

    sprintf(g_path_bounding_box, "%s/bounding_box.txt", output_dest);
    sprintf(g_path_tracks, "%s/tracks.txt", output_dest);

    if (stat(output_dest, &status) == -1) {
        mkdir(output_dest, 0700);
    }
}

void create_bb_file(char* filename) {
    sprintf(g_path_bounding_box, "%s", filename);
}

void create_frames_dir(char* dest_path) {
    struct stat status = {0};

    sprintf(g_path_frames_binary_dir, "%s/", dest_path);
    sprintf(g_path_frames_output_dir, "%s/", dest_path);

    if (stat(dest_path, &status) == -1) {
        mkdir(dest_path, 0700);
    }
    if (stat(g_path_frames_binary_dir, &status) == -1) {
        mkdir(g_path_frames_binary_dir, 0700);
    }
    if (stat(g_path_frames_output_dir, &status) == -1) {
        mkdir(g_path_frames_output_dir, 0700);
    }
}

void create_video_dir(char* dest_path) {
    struct stat status = {0};

    sprintf(g_path_video_f, "%s/", dest_path);

    if (stat(g_path_video_f, &status) == -1) {
        mkdir(g_path_video_f, 0700);
    }
}

void create_debug_files(int frame) {
    sprintf(g_path_stats_0, "%s/%05d.txt", g_path_stats_f, frame);
    sprintf(g_path_stats_1, "%s/%05d.txt", g_path_stats_f, frame + 1);
}

void create_frames_files(int frame) {
    sprintf(g_path_frames_binary, "%s/%05d.ppm", g_path_frames_binary_dir, frame);
    sprintf(g_path_frames_output, "%s/%05d.ppm", g_path_frames_output_dir, frame);
}

void create_videos_files(char* filename) {
    sprintf(g_path_video_tracking, "%s", filename);
}

void copy_ui8matrix_ui8matrix(uint8** X, int i0, int i1, int j0, int j1, uint8** Y) {
    int i, j;

    for (i = i0; i <= i1; i++) {
        for (j = j0; j <= j1; j++) {
            Y[i][j] = X[i][j];
        }
    }
}

void convert_ui8vector_ui32vector(uint8* X, long nl, long nh, uint32* Y) {
    long i;
    for (i = nl; i <= nh; i++)
        Y[i] = (uint32)X[i];
}

void convert_ui8matrix_ui32matrix(uint8** X, int nrl, int nrh, int ncl, int nch, uint32** Y) {
    long i;
    for (i = nrl; i <= nrh; i++) {
        convert_ui8vector_ui32vector(X[i], ncl, nch, Y[i]);
    }
}

void WritePNMrow(uint8* line, int width, FILE* file) {
    /* Le fichier est deja ouvert et ne sera pas ferme a la fin */
    fwrite(&(line[0]), sizeof(byte), 3 * sizeof(byte) * width, file);
}
