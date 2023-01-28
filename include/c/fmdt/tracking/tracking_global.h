#pragma once

#include "fmdt/image/image_struct.h"
#include "fmdt/features/features_struct.h"
#include "fmdt/tracking/tracking_struct.h"

#define METEOR_COLOR GREEN
#define STAR_COLOR PURPLE
#define NOISE_COLOR ORANGE
#define UNKNOWN_COLOR GRAY

#define METEOR_STR "meteor"
#define STAR_STR "star"
#define NOISE_STR "noise"
#define UNKNOWN_STR "unknown"

#define TOO_BIG_ANGLE_STR "too big angle"
#define WRONG_DIRECTION_STR "wrong direction"
#define TOO_LONG_DURATION_STR "too long duration"

extern enum color_e g_obj_to_color[N_OBJECTS];
extern char g_obj_to_string[N_OBJECTS][64];
extern char g_obj_to_string_with_spaces[N_OBJECTS][64];
extern char g_change_state_to_string[N_REASONS][64];
extern char g_change_state_to_string_with_spaces[N_REASONS][64];

void tracking_init_global_data();
enum obj_e tracking_string_to_obj_type(const char* string);
