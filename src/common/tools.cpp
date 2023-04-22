#include <stdio.h>
#include <string.h>
#include <vec.h>
#include <vector>

#include "fmdt/tools.hpp"

std::vector<std::size_t> tools_convert_int_cvector_int_stdvector(const vec_int arg) {
    std::vector<std::size_t> v;
    int size = vector_size(arg);
    
    for (int i = 0; i < size; i++)
        v.emplace_back(arg[i]);

    return v;
}

void tools_int_stdvector_print(FILE* stream, const std::vector<std::size_t> v) {
    int size = v.size();
    fprintf(stream, "[");
    for (int i = 0; i < size - 1; i++) {
        fprintf(stream, "%ld, ", v[i]);
    }
    fprintf(stream, "%ld]", v[size - 1]);
}
