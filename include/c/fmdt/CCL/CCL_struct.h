/*!
 * \file
 * \brief Connected-Component Labeling (CCL) structures.
 */

#pragma once

#include <stdint.h>

/**
 *  Internal CCL data required to perform labeling.
 */
typedef struct {
    int i0; /**< First y index in the image (included). */
    int i1; /**< Last y index in the image (included) */
    int j0; /**< First x index in the image (included) */
    int j1; /**< Last x index in the image (included) */
    uint32_t** er;  /**< Relative labels */
    //uint32_t** ea;  // Absolute labels
    uint32_t** era; /**< Relative/Absolute labels equivalences */
    uint32_t** rlc; /**< Run-length coding */
    uint32_t* eq;   /**< Table of equivalence */
    uint32_t* ner;  /**< Number of relative labels */
} CCL_data_t;
