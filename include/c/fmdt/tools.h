/*!
 * \file
 * \brief System and conversion functions.
 */

#pragma once

#include <stdio.h>
#include <stdint.h>

#include "fmdt/image/image_struct.h"

typedef int* vec_int_t;

/**
 * Copy a 2D array.
 * @param X Input matrix (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 First \f$y\f$ index in the 2D array (included).
 * @param i1 Last \f$y\f$ index in the 2D array (included).
 * @param j0 First \f$x\f$ index in the 2D array (included).
 * @param j1 Last \f$x\f$ index in the 2D array (included).
 * @param Y Output matrix (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 */
void tools_copy_ui8matrix_ui8matrix(const uint8_t** X, const int i0, const int i1, const int j0, const int j1,
                                    uint8_t** Y);

/**
 * Convert a 8-bit 2D array in a 32-bit 2D array.
 * @param X Input 8-bit matrix (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param nrl First \f$y\f$ index in the 2D array (included).
 * @param nrh Last \f$y\f$ index in the 2D array (included).
 * @param ncl First \f$x\f$ index in the 2D array (included).
 * @param nch Last \f$x\f$ index in the 2D array (included).
 * @param Y Output 32-bit matrix (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 */
void tools_convert_ui8matrix_ui32matrix(const uint8_t** X, const int nrl, const int nrh, const int ncl, const int nch,
                                        uint32_t** Y);

/**
 * Convert a 1D (linear) array into a 2D array (8-bit integers).
 * @param X Input 1D array (1D array \f$[(i1 - i0 + 1) \times (j1 - j0 + 1)]\f$).
 * @param i0 First \f$y\f$ index in the 2D array (included).
 * @param i1 Last \f$y\f$ index in the 2D array (included).
 * @param j0 First \f$x\f$ index in the 2D array (included).
 * @param j1 Last \f$x\f$ index in the 2D array (included).
 * @param Y Output matrix (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 */
void tools_linear_2d_nrc_ui8matrix(const uint8_t* X, const int i0, const int i1, const int j0, const int j1,
                                   const uint8_t** Y);

/**
 * Convert a 1D (linear) array into a 2D array (32-bit integers).
 * @param X Input 1D array (1D array \f$[(i1 - i0 + 1) \times (j1 - j0 + 1)]\f$).
 * @param i0 First \f$y\f$ index in the 2D array (included).
 * @param i1 Last \f$y\f$ index in the 2D array (included).
 * @param j0 First \f$x\f$ index in the 2D array (included).
 * @param j1 Last \f$x\f$ index in the 2D array (included).
 * @param Y Output matrix (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 */
void tools_linear_2d_nrc_ui32matrix(const uint32_t* X, const int i0, const int i1, const int j0, const int j1,
                                    const uint32_t** Y);

/**
 * Convert a 1D (linear) array into a 2D array (32-bit float).
 * @param X Input 1D array (1D array \f$[(i1 - i0 + 1) \times (j1 - j0 + 1)]\f$).
 * @param i0 First \f$y\f$ index in the 2D array (included).
 * @param i1 Last \f$y\f$ index in the 2D array (included).
 * @param j0 First \f$x\f$ index in the 2D array (included).
 * @param j1 Last \f$x\f$ index in the 2D array (included).
 * @param Y Output matrix (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 */
void tools_linear_2d_nrc_f32matrix(const float* X, const int i0, const int i1, const int j0, const int j1,
                                   const float** Y);

/**
 * Convert a 1D (linear) array into a 2D array (24-bit RGB).
 * @param X Input 1D array (1D array \f$[(i1 - i0 + 1) \times (j1 - j0 + 1)]\f$).
 * @param i0 First \f$y\f$ index in the 2D array (included).
 * @param i1 Last \f$y\f$ index in the 2D array (included).
 * @param j0 First \f$x\f$ index in the 2D array (included).
 * @param j1 Last \f$x\f$ index in the 2D array (included).
 * @param Y Output matrix (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 */
void tools_linear_2d_nrc_rgb8matrix(const rgb8_t* X, const int i0, const int i1, const int j0, const int j1,
                                    const rgb8_t** Y);

/**
 * System function to create a folder.
 * @param folder_path Path to the folder to create.
 */
void tools_create_folder(const char* folder_path);

/**
 * System function to check if a path is a directory.
 * @param path Path.
 * @return `1` if the given path is a folder, `0` otherwise.
 */
int tools_is_dir(const char *path);

/**
 * Convert a string of int into 1D (linear) array.
 * @param arg Input string (ex: \f$[1, 5, 1]\f$).
 * @param res Output 1D (linear) array.
 */
void tools_convert_string_to_int_cvector(const char* arg, vec_int_t *res);

/**
 * Display the values of a 1D (linear) array (ex: \f$[1, 5, 1]\f$).
 * @param stream Output stream.
 * @param tab Input 1D (linear) array.
 */
void tools_int_cvector_print(FILE* stream, vec_int_t tab);

/**
 * Convert a int 1D (linear) array to string.
 * @param arg Input 1D (linear) array.
 * @param res Output string (ex: \f$[1, 5, 1]\f$).
 */
void tools_convert_int_vector_to_string(vec_int_t tab, char *res);