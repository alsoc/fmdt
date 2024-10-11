#pragma once

#include "fmdt/spu/Features/Features_ellipse.hpp"

spu::runtime::Task& Features_ellipse::operator[](const ftr_ell::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Features_ellipse::operator[](const ftr_ell::sck::compute s) {
    return spu::module::Module::operator[]((size_t)ftr_ell::tsk::compute)[(size_t)s];
}

spu::runtime::Socket& Features_ellipse::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
