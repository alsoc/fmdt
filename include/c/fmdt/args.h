/*!
 * \file
 * \brief Command Line Interface (CLI) functions.
 */

#pragma once

void args_del(int argc, char** argv, int index);
int args_find(int argc, char* argv[], const char* arg);
int args_find_int(int argc, char** argv, const char* arg, int def);
int args_find_int_min_max(int argc, char** argv, const char* arg, int def, int min, int max);
int args_find_int_min(int argc, char** argv, const char* arg, int def, int min);
int args_find_int_max(int argc, char** argv, const char* arg, int def, int max);
float args_find_float(int argc, char** argv, const char* arg, float def);
float args_find_float_min_max(int argc, char** argv, const char* arg, float def, float min, float max);
float args_find_float_min(int argc, char** argv, const char* arg, float def, float min);
float args_find_float_max(int argc, char** argv, const char* arg, float def, float max);
char* args_find_char(int argc, char** argv, const char* arg, char* def);
