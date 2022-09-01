// ------------------
// --- Args.h ---
// ------------------

#pragma once

void args_del(int argc, char** argv, int index);
int args_find(int argc, char* argv[], const char* arg);
int args_find_int(int argc, char** argv, const char* arg, int def);
float args_find_float(int argc, char** argv, const char* arg, float def);
char* args_find_char(int argc, char** argv, const char* arg, char* def);
