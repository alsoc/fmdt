/*!
 * \file
 * \brief System and conversion functions.
 */

#pragma once

#include <vector>
#include <fmdt/tools.h>

/**
 * Convert int cvector into int std::vector.
 * @param arg Input int cvector.
 * @return Data converted in int std::vector.
 */
std::vector<std::size_t> tools_convert_int_cvector_int_stdvector(const vec_int_t arg);

/**
 * Convert int cvector into bool std::vector.
 * @param arg Input int cvector.
 * @return Data converted in bool std::vector.
 */
std::vector<bool> tools_convert_int_cvector_bool_stdvector(const vec_int_t arg);
