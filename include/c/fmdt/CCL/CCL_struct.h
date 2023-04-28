/*!
 * \file
 * \brief Connected-Component Labeling (CCL) structures.
 */

#pragma once

#include <stdint.h>

/**
 *  Inner CCL data required to perform labeling (for Arthur HENNEQUIN's LSL implementation).
 */
typedef struct {
    int i0; /**< First \f$y\f$ index in the image (included). */
    int i1; /**< Last \f$y\f$ index in the image (included). */
    int j0; /**< First \f$x\f$ index in the image (included). */
    int j1; /**< Last \f$x\f$ index in the image (included). */
    uint32_t** er;  /**< Relative labels. */
    //uint32_t** ea;  // Absolute labels.
    uint32_t** era; /**< Relative <-> absolute labels equivalences. */
    uint32_t** rlc; /**< Run-length coding. */
    uint32_t* eq;   /**< Table of equivalence. */
    uint32_t* ner;  /**< Number of relative labels. */
} CCL_data_t;

/**
 *  Enumeration to select CCL implementation.
 */
enum ccl_impl_e { LSLH = 0, /*!< LSL implementation from Arthur HENNEQUIN. */
                  LSLM /*!< LSL implementation from Florian LEMAITRE and Nathan MAURICE. */
};

/**
 *  Generic structure to support different CCL implementations.
 */
typedef struct {
    enum ccl_impl_e impl; /*!< Selected implementation. */
    void* metadata; /*!< Inner metadata according to the selected implementation. */
} CCL_gen_data_t;

/**
 * Convert a string into an `ccl_impl_e` enum value.
 * @param str String that can be "LSLH" or "LSLM" (if the code has been linked with the LSL library).
 * @return Corresponding enum value.
 */
enum ccl_impl_e CCL_str_to_enum(const char* str);
