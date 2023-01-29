#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"

#include "fmdt/tracking/tracking_global.h"

enum color_e g_obj_to_color[N_OBJECTS];
char g_obj_to_string[N_OBJECTS][64];
char g_obj_to_string_with_spaces[N_OBJECTS][64];
char g_change_state_to_string[N_REASONS][64];
char g_change_state_to_string_with_spaces[N_REASONS][64];

void tracking_init_global_data() {
    g_obj_to_color[UNKNOWN] = UNKNOWN_COLOR;
    g_obj_to_color[STAR] = STAR_COLOR;
    g_obj_to_color[METEOR] = METEOR_COLOR;
    g_obj_to_color[NOISE] = NOISE_COLOR;

    char str_unknown[64] = UNKNOWN_STR;
    snprintf(g_obj_to_string[UNKNOWN], sizeof(g_obj_to_string[UNKNOWN]), "%s", str_unknown);
    char str_star[64] = STAR_STR;
    snprintf(g_obj_to_string[STAR], sizeof(g_obj_to_string[STAR]), "%s", str_star);
    char str_meteor[64] = METEOR_STR;
    snprintf(g_obj_to_string[METEOR], sizeof(g_obj_to_string[METEOR]), "%s", str_meteor);
    char str_noise[64] = NOISE_STR;
    snprintf(g_obj_to_string[NOISE], sizeof(g_obj_to_string[NOISE]), "%s", str_noise);

    unsigned max = 0;
    for (int i = 0; i < N_OBJECTS; i++)
        if (strlen(g_obj_to_string[i]) > max)
            max = strlen(g_obj_to_string[i]);

    for (int i = 0; i < N_OBJECTS; i++) {
        int len = strlen(g_obj_to_string[i]);
        int diff = max - len;
        for (int c = len; c >= 0; c--)
            g_obj_to_string_with_spaces[i][diff + c] = g_obj_to_string[i][c];
        for (int c = 0; c < diff; c++)
            g_obj_to_string_with_spaces[i][c] = ' ';
    }

    snprintf(g_change_state_to_string[UNKNOWN], sizeof(g_change_state_to_string[UNKNOWN]), "%s", str_unknown);
    char str_too_big_angle[64] = TOO_BIG_ANGLE_STR;
    snprintf(g_change_state_to_string[REASON_TOO_BIG_ANGLE], sizeof(g_change_state_to_string[REASON_TOO_BIG_ANGLE]),
        "%s", str_too_big_angle);
    char str_wrong_direction[64] = WRONG_DIRECTION_STR;
    snprintf(g_change_state_to_string[REASON_WRONG_DIRECTION], sizeof(g_change_state_to_string[REASON_WRONG_DIRECTION]),
        "%s", str_wrong_direction);
    char str_too_long_duration[64] = TOO_LONG_DURATION_STR;
    snprintf(g_change_state_to_string[REASON_TOO_LONG_DURATION],
        sizeof(g_change_state_to_string[REASON_TOO_LONG_DURATION]), "%s", str_too_long_duration);

    max = 0;
    for (int i = 0; i < N_REASONS; i++)
        if (strlen(g_change_state_to_string[i]) > max)
            max = strlen(g_change_state_to_string[i]);

    for (int i = 0; i < N_REASONS; i++) {
        int len = strlen(g_change_state_to_string[i]);
        int diff = max - len;
        for (int c = len; c >= 0; c--)
            g_change_state_to_string_with_spaces[i][diff + c] = g_change_state_to_string[i][c];
        for (int c = 0; c < diff; c++)
            g_change_state_to_string_with_spaces[i][c] = ' ';
    }
}

enum obj_e tracking_string_to_obj_type(const char* string) {
    enum obj_e obj = UNKNOWN;
    for (int i = 0; i < N_OBJECTS; i++)
        if (!strcmp(string, g_obj_to_string[i])) {
            obj = (enum obj_e)i;
            break;
        }
    return obj;
}
