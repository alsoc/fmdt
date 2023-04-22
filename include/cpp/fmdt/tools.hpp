/*!
 * \file
 * \brief System and conversion functions.
 */

#pragma once

#include <vector>
#include <vec.h>

/**
 * Convert cvector of int into std::vector.
 * @param arg Input cvector of int.
 * @return Data converted in std::vector.
 */
std::vector<std::size_t> tools_convert_int_cvector_stdvector(const vec_int arg);

/**
 * Display the values of a std::vector (ex: \f$[1, 5, 1]\f$).
 * @param stream Output stream.
 * @param v Input std::vector.
 */
void tools_stdvector_print(FILE* stream, std::vector<std::size_t> v);