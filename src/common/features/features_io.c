#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "vec.h"

#include "fmdt/tracking/tracking_struct.h"
#include "fmdt/tracking/tracking_global.h"
#include "fmdt/features/features_io.h"

int find_corresponding_track(const int frame, const vec_track_t tracks, const RoI_basic_t* RoIs_basic,
                             const size_t n_RoIs, const int sel_RoIs_id, const unsigned age) {
    assert(age == 0 || age == 1);

    size_t n_tracks = vector_size(tracks);
    for (size_t t = 0; t < n_tracks; t++) {
        if (tracks[t].id) {
            if (tracks[t].end.frame == frame + age) {
                int cur_RoIs_id;
                if (age == 0)
                    cur_RoIs_id = tracks[t].end.id;
                else {
                    if (tracks[t].end.prev_id == 0)
                        continue;
                    cur_RoIs_id = RoIs_basic[tracks[t].end.prev_id - 1].id;
                }
                assert(cur_RoIs_id <= (int)n_RoIs);
                if (cur_RoIs_id <= 0)
                    continue;
                if (sel_RoIs_id == cur_RoIs_id)
                    return t;
            }
        }
    }
    return -1;
}

void features_RoIs_write(FILE* f, const int frame, const RoI_basic_t* RoIs_basic, const RoI_magn_t* RoIs_magn,
                         const RoI_elli_t* RoIs_elli, const size_t n_RoIs, const vec_track_t tracks,
                         const unsigned age) {
    int cpt = 0;
    for (size_t i = 0; i < n_RoIs; i++)
        if (RoIs_basic[i].id != 0)
            cpt++;

    fprintf(f, "Regions of interest (RoI) [%d]: \n", cpt);
    // if (cpt) {  // for compare.py
    if (tracks) {
        fprintf(f, "# ------||----------------||---------------------------||--------------------------------------------------------------------------||-------------------||-----------||------------||--------------------------\n");
        fprintf(f, "#   RoI ||      Track     ||        Bounding Box       ||                          Surface (S in pixels)                           ||      Center       || Magnitude || Saturation ||         Ellipse          \n");
        fprintf(f, "# ------||----------------||---------------------------||--------------------------------------------------------------------------||-------------------||-----------||------------||--------------------------\n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-------|----------|----------|--------------|--------------|--------------||---------|---------||-----------||------------||--------|--------|--------\n");
        fprintf(f, "#    ID ||   ID |    Type || xmin | xmax | ymin | ymax ||     S |       Sx |       Sy |          Sx2 |          Sy2 |          Sxy ||       x |       y ||        -- ||    Counter ||      a |      b |  ratio \n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-------|----------|----------|--------------|--------------|--------------||---------|---------||-----------||------------||--------|--------|--------\n");
    } else {
        fprintf(f, "# ------||---------------------------||--------------------------------------------------------------------------||-------------------||-----------||------------||--------------------------\n");
        fprintf(f, "#   RoI ||        Bounding Box       ||                          Surface (S in pixels)                           ||      Center       || Magnitude || Saturation ||         Ellipse          \n");
        fprintf(f, "# ------||---------------------------||--------------------------------------------------------------------------||-------------------||-----------||------------||--------------------------\n");
        fprintf(f, "# ------||------|------|------|------||-------|----------|----------|--------------|--------------|--------------||---------|---------||-----------||------------||--------|--------|--------\n");
        fprintf(f, "#    ID || xmin | xmax | ymin | ymax ||     S |       Sx |       Sy |          Sx2 |          Sy2 |          Sxy ||       x |       y ||        -- ||    Counter ||      a |      b |  ratio \n");
        fprintf(f, "# ------||------|------|------|------||-------|----------|----------|--------------|--------------|--------------||---------|---------||-----------||------------||--------|--------|--------\n");
    }
    // }

    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_basic[i].id != 0) {
            int t = tracks ? find_corresponding_track(frame, tracks, RoIs_basic, n_RoIs, RoIs_basic[i].id, age) : -1;
            char task_id_str[16];
            if (t == -1)
                strcpy(task_id_str, "   -");
            else
                snprintf(task_id_str, sizeof(task_id_str), "%4u", tracks[t].id);
            char task_obj_type[64];
            if (t == -1)
                strcpy(task_obj_type, "      -");
            else
                snprintf(task_obj_type, sizeof(task_obj_type), "%s",
                    g_obj_to_string_with_spaces[tracks[t].obj_type]);

            char mag_str[16], satc_str[16];
            if (RoIs_magn) {
                snprintf(mag_str, sizeof(mag_str), "%9u", RoIs_magn[i].magnitude);
                snprintf(satc_str, sizeof(satc_str), "%10u", RoIs_magn[i].sat_count);
            } else {
                strcpy(mag_str,  "        -");
                strcpy(satc_str, "         -");
            }

            char a_str[16], b_str[16], ratio_str[16];
            if (RoIs_elli) {
                snprintf(a_str, sizeof(a_str), "%6.2f", RoIs_elli[i].a);
                snprintf(b_str, sizeof(b_str), "%6.2f", RoIs_elli[i].b);
                float ratio = (RoIs_elli[i].b == 0.0f) ? RoIs_elli[i].a : RoIs_elli[i].a / RoIs_elli[i].b;
                snprintf(ratio_str, sizeof(ratio_str), "%6.2f", ratio);
            } else {
                strcpy(a_str,     "     -");
                strcpy(b_str,     "     -");
                strcpy(ratio_str, "     -");
            }

            if (tracks) {
                fprintf(f, "   %4u || %s | %s || %4u | %4u | %4u | %4u || %5u | %8u | %8u | %12u | %12u | %12u || %7.1f | %7.1f || %s || %s || %s | %s | %s \n",
                        RoIs_basic[i].id, task_id_str, task_obj_type, RoIs_basic[i].xmin, RoIs_basic[i].xmax,
                        RoIs_basic[i].ymin, RoIs_basic[i].ymax, RoIs_basic[i].S, RoIs_basic[i].Sx, RoIs_basic[i].Sy,
                        (uint32_t)RoIs_basic[i].Sx2, (uint32_t)RoIs_basic[i].Sy2, (uint32_t)RoIs_basic[i].Sxy,
                        RoIs_basic[i].x, RoIs_basic[i].y, mag_str, satc_str, a_str, b_str, ratio_str);
            } else {
                fprintf(f, "   %4u || %4u | %4u | %4u | %4u || %5u | %8u | %8u | %12u | %12u | %12u || %7.1f | %7.1f || %s || %s || %s | %s | %s \n",
                        RoIs_basic[i].id, RoIs_basic[i].xmin, RoIs_basic[i].xmax, RoIs_basic[i].ymin,
                        RoIs_basic[i].ymax, RoIs_basic[i].S, RoIs_basic[i].Sx, RoIs_basic[i].Sy,
                        (uint32_t)RoIs_basic[i].Sx2, (uint32_t)RoIs_basic[i].Sy2, (uint32_t)RoIs_basic[i].Sxy,
                        RoIs_basic[i].x, RoIs_basic[i].y, mag_str, satc_str, a_str, b_str, ratio_str);
            }
        }
    }
}

void features_RoIs0_RoIs1_write(FILE* f, const int prev_frame, const int cur_frame, const RoI_basic_t* RoIs0_basic,
                                const RoI_magn_t* RoIs0_magn, const RoI_elli_t* RoIs0_elli, const size_t n_RoIs0,
                                const RoI_basic_t* RoIs1_basic, const RoI_magn_t* RoIs1_magn,
                                const RoI_elli_t* RoIs1_elli, const size_t n_RoIs1, const vec_track_t tracks) {
    if (prev_frame >= 0) {
        fprintf(f, "# Frame n°%05d (t-1) -- ", prev_frame);
        features_RoIs_write(f, prev_frame, RoIs0_basic, RoIs0_magn, RoIs0_elli, n_RoIs0, tracks, 1);
        fprintf(f, "#\n");
    }

    fprintf(f, "# Frame n°%05d (t) -- ", cur_frame);
    features_RoIs_write(f, cur_frame, RoIs1_basic, RoIs1_magn, RoIs1_elli, n_RoIs1, tracks, 0);
}
