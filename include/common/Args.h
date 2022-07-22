// ------------------
// --- Args.h ---
// ------------------

#ifndef _ARGS_
#define _ARGS_

void del_arg(int argc, char **argv, int index);
int find_arg(int argc, char* argv[], const char *arg);
int find_int_arg(int argc, char **argv, const char *arg, int def);
float find_float_arg(int argc, char **argv, const char *arg, float def);
char* find_char_arg(int argc, char **argv, const char *arg, char* def);

#endif // _ARGS_
