#pragma once

#include "fmdt/spu/Features/Features_magnitude.hpp"

spu::runtime::Task& Features_magnitude::operator[](const ftr_mgn::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Features_magnitude::operator[](const ftr_mgn::sck::compute s) {
    return spu::module::Module::operator[]((size_t)ftr_mgn::tsk::compute)[(size_t)s];
}

spu::runtime::Socket& Features_magnitude::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
