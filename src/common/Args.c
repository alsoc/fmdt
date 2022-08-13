/**
 * Copyright (c) 2017-2018, Arthur Hennequin
 * LIP6, UPMC, CNRS
 */

#include <stdlib.h>
#include <string.h>

#include "Args.h"

void del_arg(int argc, char** argv, int index) {
    int i;
    for (i = index; i < argc - 1; i++)
        argv[i] = argv[i + 1];
    argv[i] = 0;
}

int find_arg(int argc, char* argv[], const char* arg) {
    for (int i = 0; i < argc; i++) {
        if (!argv[i])
            continue;
        if (strcmp(argv[i], arg) == 0) {
            // del_arg(argc, argv, i);
            return 1;
        }
    }
    return 0;
}

int find_int_arg(int argc, char** argv, const char* arg, int def) {
    for (int i = 0; i < argc - 1; i++) {
        if (!argv[i])
            continue;
        if (strcmp(argv[i], arg) == 0) {
            def = atoi(argv[i + 1]);
            // del_arg(argc, argv, i);
            // del_arg(argc, argv, i);
            break;
        }
    }
    return def;
}

float find_float_arg(int argc, char** argv, const char* arg, float def) {
    for (int i = 0; i < argc - 1; i++) {
        if (!argv[i])
            continue;
        if (strcmp(argv[i], arg) == 0) {
            def = atof(argv[i + 1]);
            // del_arg(argc, argv, i);
            // del_arg(argc, argv, i);
            break;
        }
    }
    return def;
}

char* find_char_arg(int argc, char** argv, const char* arg, char* def) {
    for (int i = 0; i < argc - 1; i++) {
        if (!argv[i])
            continue;
        if (strcmp(argv[i], arg) == 0) {
            def = argv[i + 1];
            // del_arg(argc, argv, i);
            // del_arg(argc, argv, i);
            break;
        }
    }
    return def;
}
