#include <stdio.h>
#include <stdlib.h>

#include "vec.h"

#include "fmdt/tracking/tracking_struct.h"

size_t _tracking_get_track_time(const RoI_t track_begin, const RoI_t track_end) {
    return track_end.frame - track_begin.frame;
}

size_t tracking_get_track_time(const vec_track_t track_array, const size_t t) {
    return _tracking_get_track_time(track_array[t].begin, track_array[t].end);
}

size_t tracking_count_objects(const vec_track_t track_array, unsigned* n_stars, unsigned* n_meteors,
                              unsigned* n_noise) {
    size_t n_tracks = vector_size(track_array);
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for (size_t i = 0; i < n_tracks; i++)
        if (track_array[i].id)
            switch (track_array[i].obj_type) {
            case OBJ_STAR:
                (*n_stars)++;
                break;
            case OBJ_METEOR:
                (*n_meteors)++;
                break;
            case OBJ_NOISE:
                (*n_noise)++;
                break;
            default:
                fprintf(stderr, "(EE) This should never happen ('track_array[i].obj_type = %d', 'i = %lu')\n",
                        track_array[i].obj_type, (unsigned long)i);
                exit(1);
            }
    return (*n_stars) + (*n_meteors) + (*n_noise);
}
