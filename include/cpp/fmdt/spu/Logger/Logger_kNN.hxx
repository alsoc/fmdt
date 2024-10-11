#pragma once

#include "fmdt/spu/Logger/Logger_kNN.hpp"

spu::runtime::Task& Logger_kNN::operator[](const lgr_knn::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Logger_kNN::operator[](const lgr_knn::sck::write s) {
    return spu::module::Module::operator[]((size_t)lgr_knn::tsk::write)[(size_t)s];
}

spu::runtime::Socket& Logger_kNN::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
