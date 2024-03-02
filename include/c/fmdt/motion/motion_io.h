/*!
 * \file
 * \brief IOs for global motion estimation.
 */

#pragma once

#include <stdio.h>

#include "fmdt/motion/motion_struct.h"

/**
 * Print a table of global motion estimation.
 *
 * @param f File descriptor (in write mode).
 * @param motion_est1 First global motion estimation.
 * @param motion_est2 Last global motion estimation.
 */
void motion_write(FILE* f, const motion_t* motion_est1, const motion_t* motion_est2);
