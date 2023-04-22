/*!
 * \file
 * \brief Command Line Interface (CLI) functions.
 *
 *  Note that this simple CLI library follows POSIX conventions:
 *    - single character arguments start with the '-' character (ex.: "-h"),
 *    - multiple characters arguments start with the "--" characters (ex.: "--help"),
 *    - argument and its value is separated with a blank character (ex.: "--min-val 12"),
 *    - for boolean arguments, no argument is expected (ex.: if "-h" is found, then the help is printed).
 */

#pragma once

#include "vec.h"

void args_del(int argc, char** argv, int index);

/**
 * Find if an argument exists in program command line.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @return `1` if the argument is found, `0` otherwise.
 */
int args_find(int argc, char** argv, const char* arg);

/**
 * Find an argument and return its corresponding value as an integer value.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @return Value corresponding to the argument if it exists in the command line, \p def value otherwise.
 */
int args_find_int(int argc, char** argv, const char* arg, int def);

/**
 * Find an argument and return its corresponding value as an integer value.
 * This function also tests that the returned value is between the \f$[min;max]\f$ range. If it is not the case,
 * it prints an error message and exits the program with `-1` value.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @param min Minimum accepted value.
 * @param max Maximum accepted value.
 * @return Value corresponding to the argument if it exists in the command line, \p def value otherwise.
 */
int args_find_int_min_max(int argc, char** argv, const char* arg, int def, int min, int max);

/**
 * Find an argument and return its corresponding value as an integer value.
 * This function also tests that the returned value is higher (or equal) than a minimum value. If it is not the case,
 * it prints an error message and exits the program with `-1` value.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @param min Minimum accepted value.
 * @return Value corresponding to the argument if it exists in the command line, \p def value otherwise.
 */
int args_find_int_min(int argc, char** argv, const char* arg, int def, int min);

/**
 * Find an argument and return its corresponding value as an integer value.
 * This function also tests that the returned value is lower (or equal) than a maximum value. If it is not the case,
 * it prints an error message and exits the program with `-1` value.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @param max Maximum accepted value.
 * @return Value corresponding to the argument if it exists in the command line, \p def value otherwise.
 */
int args_find_int_max(int argc, char** argv, const char* arg, int def, int max);

/**
 * Find an argument and return its corresponding value as a floating-point value.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @return Value corresponding to the argument if it exists in the command line, \p def value otherwise.
 */
float args_find_float(int argc, char** argv, const char* arg, float def);

/**
 * Find an argument and return its corresponding value as a floating-point value.
 * This function also tests that the returned value is between the \f$[min;max]\f$ range. If it is not the case,
 * it prints an error message and exits the program with `-1` value.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @param min Minimum accepted value.
 * @param max Maximum accepted value.
 * @return Value corresponding to the argument if it exists in the command line, \p def value otherwise.
 */
float args_find_float_min_max(int argc, char** argv, const char* arg, float def, float min, float max);

/**
 * Find an argument and return its corresponding value as a floating-point value.
 * This function also tests that the returned value is higher (or equal) than a minimum value. If it is not the case,
 * it prints an error message and exits the program with `-1` value.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @param min Minimum accepted value.
 * @return Value corresponding to the argument if it exists in the command line, \p def value otherwise.
 */
float args_find_float_min(int argc, char** argv, const char* arg, float def, float min);

/**
 * Find an argument and return its corresponding value as a floating-point value.
 * This function also tests that the returned value is lower (or equal) than a maximum value. If it is not the case,
 * it prints an error message and exits the program with `-1` value.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @param max Maximum accepted value.
 * @return Value corresponding to the argument if it exists in the command line, \p def value otherwise.
 */
float args_find_float_max(int argc, char** argv, const char* arg, float def, float max);

/**
 * Find an argument and return its corresponding value as string (array of characters).
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @return Pointer of characters in \p argv corresponding to the argument value if it exists in the command line, \p def
 *         pointer otherwise.
 */
char* args_find_char(int argc, char** argv, const char* arg, char* def);

/**
 * Find an argument and return its corresponding value as a vector of int.
 * @param argc Number of arguments in \p argv array of arguments.
 * @param argv Array of arguments.
 * @param arg Argument to look for. Note that a list of arguments can be provided: arguments have to be separated by a
 *            comma (',') character.
 * @param def Default value if the argument is not found.
 * @param res Pointer where the value  will be saved if the argument is found.
 * @return Pointer on \p res corresponding to the argument value if it exists in the command line, \p def
 *         value otherwise.
 */
vec_int args_find_vector_int(int argc, char** argv, const char* arg, vec_int def, vec_int res);