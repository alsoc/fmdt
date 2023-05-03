#include <stdio.h>
#include <string.h>
#include <vec.h>
#include <vector>

#include "fmdt/tools.hpp"

std::vector<std::size_t> tools_convert_int_cvector_int_stdvector(const vec_int_t arg) {
    std::vector<std::size_t> v;
    int size = vector_size(arg);
    
    for (int i = 0; i < size; i++)
        v.emplace_back(arg[i]);

    return v;
}

std::vector<bool> tools_convert_int_cvector_bool_stdvector(const vec_int_t arg) {
    std::vector<bool> v;
    int size = vector_size(arg);
    
    for (int i = 0; i < size; i++)
        v.emplace_back(arg[i] ? true : false);

    return v;
}

std::vector<std::vector<std::size_t>> tools_convert_int_cvector2D_int_stdvector2D(const vec2D_int_t arg) {
    std::vector<std::vector<std::size_t>> res;
    std::vector<std::size_t> vec;
    int size = vector_size(arg);
    
    for (int i = 0; i < size; i++) {
        vec.clear();

        int sub_size = vector_size(arg[i]);
        for (int j = 0; j < sub_size; j++) {
            vec.push_back(arg[i][j]);
        }

        res.push_back(vec);
    }
    
    return res;
}