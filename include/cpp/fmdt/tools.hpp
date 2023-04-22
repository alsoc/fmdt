/*!
 * \file
 * \brief System and conversion functions.
 */

#pragma once

#include <vector>
#include <vec.h>

/**
 * Convert int cvector into int std::vector.
 * @param arg Input int cvector.
 * @return Data converted in int std::vector.
 */
std::vector<std::size_t> tools_convert_int_cvector_int_stdvector(const vec_int arg);

/**
 * Display the int values of a std::vector (ex: \f$[1, 5, 1]\f$).
 * @param stream Output stream.
 * @param v Input int std::vector.
 */
void tools_int_stdvector_print(FILE* stream, const std::vector<std::size_t> v);
