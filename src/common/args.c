#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmdt/args.h"

void args_del(int argc, char** argv, int index) {
    int i;
    for (i = index; i < argc - 1; i++)
        argv[i] = argv[i + 1];
    argv[i] = 0;
}

int args_find(int argc, char* argv[], const char* arg) {
    for (int i = 0; i < argc; i++) {
        if (!argv[i])
            continue;
        if (strcmp(argv[i], arg) == 0) {
            // del(argc, argv, i);
            return 1;
        }
    }
    return 0;
}

int args_find_int(int argc, char** argv, const char* arg, int def) {
    for (int i = 0; i < argc - 1; i++) {
        if (!argv[i])
            continue;
        if (strcmp(argv[i], arg) == 0) {
            def = atoi(argv[i + 1]);
            // del(argc, argv, i);
            // del(argc, argv, i);
            break;
        }
    }
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
    for (int i = 0; i < argc - 1; i++) {
        if (!argv[i])
            continue;
        if (strcmp(argv[i], arg) == 0) {
            def = atof(argv[i + 1]);
            // del(argc, argv, i);
            // del(argc, argv, i);
            break;
        }
    }
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
    for (int i = 0; i < argc - 1; i++) {
        if (!argv[i])
            continue;
        if (strcmp(argv[i], arg) == 0) {
            def = argv[i + 1];
            // del(argc, argv, i);
            // del(argc, argv, i);
            break;
        }
    }
    return def;
}
