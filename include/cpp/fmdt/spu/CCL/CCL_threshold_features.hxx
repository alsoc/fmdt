#pragma once

#include "fmdt/spu/CCL/CCL_threshold_features.hpp"

uint32_t** CCL_threshold_features::get_out_labels() {
    return (*this)[ccl_tf::sck::apply::out_labels].get_2d_dataptr<uint32_t>(this->b, this->b);
}

spu::runtime::Task& CCL_threshold_features::operator[](const ccl_tf::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& CCL_threshold_features::operator[](const ccl_tf::sck::apply s) {
    return spu::module::Module::operator[]((size_t)ccl_tf::tsk::apply)[(size_t)s];
}

spu::runtime::Socket& CCL_threshold_features::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
