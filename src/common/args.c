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

vec_int_t args_find_vector_int(int argc, char** argv, const char* arg, const char* def){
    vec_int_t res = (vec_int_t)vector_create();
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
                args_convert_string_to_int_vector(argv[i + 1], &res);
                break;
            }
        }
    } while((cur_arg = strtok_r(NULL, ",", &saveptr1)) != NULL);

    if (vector_size(res) == 0){
        args_convert_string_to_int_vector(def, &res);
    }
   
    return res;
}

vec2D_int_t args_find_vector2D_int(int argc, char** argv, const char* arg, const char* def){
    vec2D_int_t res = (vec2D_int_t)vector_create();
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
                args_convert_string_to_int_vector2D(argv[i + 1], &res);
                break;
            }
        }
    } while((cur_arg = strtok_r(NULL, ",", &saveptr1)) != NULL);

    if (vector_size(res) == 0){
        args_convert_string_to_int_vector2D(def, &res);
    }
   
    return res;
}

void args_convert_string_to_int_vector(const char* arg, vec_int_t *res) {
    char *saveptr1;
    char arg_cpy[2048];
    strncpy(arg_cpy, arg, sizeof(arg_cpy));
    arg_cpy[sizeof(arg_cpy) - 1] = 0;
    char *cur_arg = arg_cpy;
    cur_arg = strtok_r(cur_arg, "[],", &saveptr1);
    do {
        vector_add(res, atoi(cur_arg));
    } while((cur_arg = strtok_r(NULL, "[],", &saveptr1)) != NULL);
}

void args_convert_string_to_int_vector2D(const char* arg, vec2D_int_t *res) {
    vec_int_t v;
    char *saveptr1;
    char arg_cpy[2048];
    strncpy(arg_cpy, arg, sizeof(arg_cpy));
    arg_cpy[sizeof(arg_cpy) - 1] = 0;
    char *cur_arg = arg_cpy;
    cur_arg = strtok_r(cur_arg, "[]", &saveptr1);
    do {
        if (strcmp(cur_arg, ",") == 0)
            cur_arg = strtok_r(NULL, "[]", &saveptr1);
        v = (vec_int_t)vector_create();
        args_convert_string_to_int_vector(cur_arg, &v);
        vector_add(res, v);
    } while((cur_arg = strtok_r(NULL, "[]", &saveptr1)) != NULL);
}

void args_convert_int_vector_to_string(vec_int_t tab, char *res) {
    char tmp[5];
    res[0] = '\0';
    int size = vector_size(tab);
    
    if (!size) return;

    sprintf(res, "[");
    for (int i = 0; i < size - 1; i++) {
        sprintf(tmp, "%d,", tab[i]);
        strcat(res, tmp);
    }
    sprintf(tmp, "%d]", tab[size - 1]);
    strcat(res, tmp);
}

void args_convert_int_vector2D_to_string(vec2D_int_t tab, char *res) {
    char tmp[20];
    res[0] = '\0';
    int size = vector_size(tab);

    sprintf(res, "[");
    for (int i = 0; i < size - 1; i++) {
        vec_int_t v = tab[i];
        args_convert_int_vector_to_string(v,tmp);
        strcat(res, tmp);
        strcat(res, ",");
    }
    args_convert_int_vector_to_string(tab[size-1],tmp);
    strcat(res, tmp);
    strcat(res, "]");
}
