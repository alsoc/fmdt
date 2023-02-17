/*!
 * \file
 * \brief Functions to apply a threshold on image.
 */

#pragma once

#include <stdint.h>

/**
 * Convert an input image (\f$I_{in}\f$) in grayscale levels into a binary image (\f$I_{out}\f$) depending on a
 * greyscale threshold (\f$T\f$).
 * If \f$ I_{in}^i \geq T\f$ then \f$I_{out}^i = 255 \f$, else \f$I_{out}^i = 0\f$.
 * @param img_in Input greyscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$).
 * @param img_out Output binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$, \f$\{0,1\}\f$ is coded
 *                as \f$\{0,255\}\f$).
 * @param i0 First \f$y\f$ index in the image (included).
 * @param i1 Last \f$y\f$ index in the image (included).
 * @param j0 First \f$x\f$ index in the image (included).
 * @param j1 Last \f$x\f$ index in the image (included).
 * @param threshold Value that define if the pixel is kept in the output binary image or not.
 */
void threshold(const uint8_t** img_in, uint8_t** img_out, const int i0, const int i1, const int j0, const int j1,
               const uint8_t threshold);
