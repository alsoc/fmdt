/*!
 * \file
 * \brief Global data related to the tracking.
 */

#pragma once

#include "fmdt/image/image_struct.h"
#include "fmdt/features/features_struct.h"
#include "fmdt/tracking/tracking_struct.h"

/** Associate the green color to a meteor */
#define METEOR_COLOR COLOR_GREEN

/** Associate the purple color to a star */
#define STAR_COLOR COLOR_PURPLE

/** Associate the orange color to noise */
#define NOISE_COLOR COLOR_ORANGE

/** Associate the gray color to unknown object */
#define UNKNOWN_COLOR COLOR_GRAY

/** Define "meteor" string */
#define METEOR_STR "meteor"

/** Define "star" string */
#define STAR_STR "star"

/** Define "noise" string */
#define NOISE_STR "noise"

/** Define "unknown" string */
#define UNKNOWN_STR "unknown"

/** Define "too big angle" string */
#define TOO_BIG_ANGLE_STR "too big angle"

/** Define "wrong direction" string */
#define WRONG_DIRECTION_STR "wrong direction"

/** Define "too long duration" string */
#define TOO_LONG_DURATION_STR "too long duration"

/** LUT to find object color from its type */
extern enum color_e g_obj_to_color[N_OBJECTS];

/** LUT to find object string from its type */
extern char g_obj_to_string[N_OBJECTS][64];

/** LUT to find object string (with spaces) from its type */
extern char g_obj_to_string_with_spaces[N_OBJECTS][64];

/** LUT to find reason string from its reason */
extern char g_change_state_to_string[N_REASONS][64];

/** LUT to find reason string (with spaces) from its reason */
extern char g_change_state_to_string_with_spaces[N_REASONS][64];

/**
 * Initialize global LUTs (`g_obj_to_color`, `g_obj_to_string`, `g_obj_to_string_with_spaces`,
 * `g_change_state_to_string` and `g_change_state_to_string_with_spaces`).
 */
void tracking_init_global_data();

/**
 * Return object type from its corresponding string.
 * @param string A string.
 * @return obj_e The right object type.
 */
enum obj_e tracking_string_to_obj_type(const char* string);
