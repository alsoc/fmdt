#pragma once

#include "fmdt/spu/Features/Features_merger_CCL_HI_v2.hpp"

uint32_t** Features_merger_CCL_HI_v2::get_out_labels() {
    return (*this)[ftr_mrg2::sck::merge::out_labels].get_2d_dataptr<uint32_t>(this->b, this->b);
}

spu::runtime::Task& Features_merger_CCL_HI_v2::operator[](const ftr_mrg2::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Features_merger_CCL_HI_v2::operator[](const ftr_mrg2::sck::merge s) {
    return spu::module::Module::operator[]((size_t)ftr_mrg2::tsk::merge)[(size_t)s];
}

spu::runtime::Socket& Features_merger_CCL_HI_v2::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
