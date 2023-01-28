#include "fmdt/validation/validation_global.h"

uint8_t g_is_valid_track[MAX_TRACKS_SIZE] = {0};
unsigned g_n_val_objects = 0;
validation_obj_t* g_val_objects = NULL;
int g_true_positive[N_OBJECTS] = {0};
int g_false_positive[N_OBJECTS] = {0};
int g_true_negative[N_OBJECTS] = {0};
int g_false_negative[N_OBJECTS] = {0};