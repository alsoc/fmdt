#pragma once

#include <stdint.h>

#include "fmdt/validation/validation_struct.h"

int validation_init(const char* val_objects_file);
void validation_process(const vec_track_t track_array);
void validation_free(void);

unsigned validation_count_objects(const validation_obj_t* val_objects, const unsigned n_val_objects, unsigned* n_stars,
                                  unsigned* n_meteors, unsigned* n_noise);
