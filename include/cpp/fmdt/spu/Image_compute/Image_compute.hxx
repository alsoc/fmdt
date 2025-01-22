
#pragma once

#include "fmdt/spu/Image_compute/Image_compute.hpp"

spu::runtime::Task& Image_compute::operator[](const imgc::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Image_compute::operator[](const imgc::sck::gray2rgb s) {
    return spu::module::Module::operator[]((size_t)imgc::tsk::gray2rgb)[(size_t)s];
}

spu::runtime::Socket& Image_compute::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
