#pragma once

#include "fmdt/spu/CCL/CCL.hpp"

uint32_t** CCL::get_out_labels() {
    return (*this)[ccl::sck::apply::out_labels].get_2d_dataptr<uint32_t>(this->b, this->b);
}

spu::runtime::Task& CCL::operator[](const ccl::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& CCL::operator[](const ccl::sck::apply s) {
    return spu::module::Module::operator[]((size_t)ccl::tsk::apply)[(size_t)s];
}

spu::runtime::Socket& CCL::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
