/*!
 * \file
 * \brief C++ conversion functions for command line arguments management.
 */

#pragma once

#include <vector>
#include <cstdint>

std::vector<std::size_t> args_find_vector_int(int argc, char** argv, const char* arg, std::vector<std::size_t> def);
