#pragma once

#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI_v3.hpp"

uint32_t** Features_merger_CCL_HI_v3::get_out_labels() {
    return (*this)[ftr_mrg3::sck::merge::out_labels].get_2d_dataptr<uint32_t>(this->b, this->b);
}

aff3ct::runtime::Task& Features_merger_CCL_HI_v3::operator[](const ftr_mrg3::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Features_merger_CCL_HI_v3::operator[](const ftr_mrg3::sck::merge s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_mrg3::tsk::merge)[(size_t)s];
}

aff3ct::runtime::Socket& Features_merger_CCL_HI_v3::operator[](const std::string &tsk_sck) {
    return aff3ct::module::Module::operator[](tsk_sck);
}
