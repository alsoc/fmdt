#include <vector>
#include <iostream>
#include <string.h>

#include "fmdt/args.hpp"

std::vector<std::size_t> convert_char2vector(const char* arg){
    char arg_cpy[2048];
    std::vector<std::size_t> v;
    strncpy(arg_cpy, arg, sizeof(arg_cpy));
    arg_cpy[sizeof(arg_cpy) - 1] = 0;
    char *cur_arg = arg_cpy;
    cur_arg = strtok(cur_arg, "[,]");
    
    do {
        v.emplace_back(atoi(cur_arg));
    } while((cur_arg = strtok(NULL, "[,]")) != NULL);

    return v;
}


std::vector<std::size_t> args_find_vector_int(int argc, char** argv, const char* arg, std::vector<std::size_t> def){
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
                def = convert_char2vector(argv[i + 1]);
                break;
            }
        }
    } while((cur_arg = strtok(NULL, ",")) != NULL);

    return def;
}