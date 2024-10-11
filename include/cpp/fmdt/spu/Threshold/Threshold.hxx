#pragma once

#include "fmdt/spu/Threshold/Threshold.hpp"

uint8_t** Threshold::get_out_img() {
    return (*this)[thr::sck::apply::out_img].get_2d_dataptr<uint8_t>(this->b, this->b);
}

spu::runtime::Task& Threshold::operator[](const thr::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Threshold::operator[](const thr::sck::apply s) {
    return spu::module::Module::operator[]((size_t)thr::tsk::apply)[(size_t)s];
}

spu::runtime::Socket& Threshold::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
