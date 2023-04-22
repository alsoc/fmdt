#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "vec.h"

void args_del(int argc, char** argv, int index) {
    int i;
    for (i = index; i < argc - 1; i++)
        argv[i] = argv[i + 1];
    argv[i] = 0;
}

int args_find(int argc, char** argv, const char* arg) {
    char arg_cpy[2048];
    strncpy(arg_cpy, arg, sizeof(arg_cpy));
    arg_cpy[sizeof(arg_cpy) - 1] = 0;
    char *cur_arg = arg_cpy;
    cur_arg = strtok(cur_arg, ",");

    do {
        for (int i = 0; i < argc; i++) {
            if (!argv[i])
                continue;
            if (strcmp(argv[i], cur_arg) == 0) {
                // del(argc, argv, i);
                return 1;
            }
        }
    } while((cur_arg = strtok(NULL, ",")) != NULL);

    return 0;
}

int args_find_int(int argc, char** argv, const char* arg, int def) {
    char arg_cpy[2048];
    strncpy(arg_cpy, arg, sizeof(arg_cpy));
    arg_cpy[sizeof(arg_cpy) - 1] = 0;
    char *cur_arg = arg_cpy;
    cur_arg = strtok(cur_arg, ",");

    do {
        for (int i = 0; i < argc - 1; i++) {
            if (!argv[i])
                continue;
            if (strcmp(argv[i], cur_arg) == 0) {
                def = atoi(argv[i + 1]);
                // del(argc, argv, i);
                // del(argc, argv, i);
                break;
            }
        }
    } while((cur_arg = strtok(NULL, ",")) != NULL);

    return def;
}

int args_find_int_min_max(int argc, char** argv, const char* arg, int def, int min, int max) {
    int arg_val = args_find_int(argc, argv, arg, def);
    if (arg_val < min || arg_val > max) {
        fprintf(stderr, "(EE) '%s' CLI argument does not match the constraint: "
                        "%d <= arg_val <= %d, arg_val = %d, exiting.\n", arg, min, max, arg_val);
        exit(-1);
    }
    return arg_val;
}

int args_find_int_min(int argc, char** argv, const char* arg, int def, int min) {
    int arg_val = args_find_int(argc, argv, arg, def);
    if (arg_val < min) {
        fprintf(stderr, "(EE) '%s' CLI argument does not match the constraint: "
                        "%d <= arg_val, arg_val = %d, exiting.\n", arg, min, arg_val);
        exit(-1);
    }
    return arg_val;
}

int args_find_int_max(int argc, char** argv, const char* arg, int def, int max) {
    int arg_val = args_find_int(argc, argv, arg, def);
    if (arg_val > max) {
        fprintf(stderr, "(EE) '%s' CLI argument does not match the constraint: "
                        "%d >= arg_val, arg_val = %d, exiting.\n", arg, max, arg_val);
        exit(-1);
    }
    return arg_val;
}

float args_find_float(int argc, char** argv, const char* arg, float def) {
    char arg_cpy[2048];
    strncpy(arg_cpy, arg, sizeof(arg_cpy));
    arg_cpy[sizeof(arg_cpy) - 1] = 0;
    char *cur_arg = arg_cpy;
    cur_arg = strtok(cur_arg, ",");

    do {
        for (int i = 0; i < argc - 1; i++) {
            if (!argv[i])
                continue;
            if (strcmp(argv[i], cur_arg) == 0) {
                def = atof(argv[i + 1]);
                // del(argc, argv, i);
                // del(argc, argv, i);
                break;
            }
        }
    } while((cur_arg = strtok(NULL, ",")) != NULL);

    return def;
}

float args_find_float_min_max(int argc, char** argv, const char* arg, float def, float min, float max) {
    float arg_val = args_find_float(argc, argv, arg, def);
    if (arg_val < min || arg_val > max) {
        fprintf(stderr, "(EE) '%s' CLI argument does not match the constraint: "
                        "%f <= arg_val <= %f, arg_val = %f, exiting.\n", arg, min, max, arg_val);
        exit(-1);
    }
    return arg_val;
}

float args_find_float_min(int argc, char** argv, const char* arg, float def, float min) {
    float arg_val = args_find_float(argc, argv, arg, def);
    if (arg_val < min) {
        fprintf(stderr, "(EE) '%s' CLI argument does not match the constraint: "
                        "%f <= arg_val, arg_val = %f, exiting.\n", arg, min, arg_val);
        exit(-1);
    }
    return arg_val;
}

float args_find_float_max(int argc, char** argv, const char* arg, float def, float max) {
    float arg_val = args_find_float(argc, argv, arg, def);
    if (arg_val > max) {
        fprintf(stderr, "(EE) '%s' CLI argument does not match the constraint: "
                "%f >= arg_val, arg_val = %f, exiting.\n", arg, max, arg_val);
        exit(-1);
    }
    return arg_val;
}

char* args_find_char(int argc, char** argv, const char* arg, char* def) {
    char arg_cpy[2048];
    strncpy(arg_cpy, arg, sizeof(arg_cpy));
    arg_cpy[sizeof(arg_cpy) - 1] = 0;
    char *cur_arg = arg_cpy;
    cur_arg = strtok(cur_arg, ",");

    do {
        for (int i = 0; i < argc - 1; i++) {
            if (!argv[i])
                continue;
            if (strcmp(argv[i], cur_arg) == 0) {
                def = argv[i + 1];
                // del(argc, argv, i);
                // del(argc, argv, i);
                break;
            }
        }
    } while((cur_arg = strtok(NULL, ",")) != NULL);

    return def;
}

vec_int args_find_vector_int(int argc, char** argv, const char* arg, vec_int def, vec_int res){
    char arg_cpy[2048];
    strncpy(arg_cpy, arg, sizeof(arg_cpy));
    arg_cpy[sizeof(arg_cpy) - 1] = 0;
    char *cur_arg = arg_cpy;
    char *saveptr1;
    cur_arg = strtok_r(cur_arg, ",", &saveptr1);

    do {
        for (int i = 0; i < argc - 1; i++) {
            if (!argv[i])
                continue;
            if (strcmp(argv[i], cur_arg) == 0) {
                tools_convert_char_int_cvector(argv[i + 1], &res);
                break;
            }
        }
    } while((cur_arg = strtok_r(NULL, ",", &saveptr1)) != NULL);

    if (vector_size(res) == 0){
        int size = vector_size(def);
        for (int i = 0; i < size; i++)
            vector_add(&res, def[i]);
    }
   
    return res;
}