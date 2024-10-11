#pragma once

#include "fmdt/spu/Features/Features_extractor.hpp"

spu::runtime::Task& Features_extractor::operator[](const ftr_ext::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Features_extractor::operator[](const ftr_ext::sck::extract s) {
    return spu::module::Module::operator[]((size_t)ftr_ext::tsk::extract)[(size_t)s];
}

spu::runtime::Socket& Features_extractor::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
