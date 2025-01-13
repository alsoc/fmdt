/*!
 * \file
 * \brief Frame structures.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "fmdt/image/image_struct.h"
#include "fmdt/features/features_struct.h"


/**
 *  Frame structure.
 */
typedef struct {
    size_t id;
    img_data_t* img;
    RoI_basic_t* RoIs;
    size_t n_RoIs;
} frame_t;
