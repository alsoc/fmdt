#pragma once

#include "fmdt/spu/kNN_matcher/kNN_matcher.hpp"

spu::runtime::Task& kNN_matcher::operator[](const knn::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& kNN_matcher::operator[](const knn::sck::match s) {
    return spu::module::Module::operator[]((size_t)knn::tsk::match)[(size_t)s];
}

spu::runtime::Socket& kNN_matcher::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
