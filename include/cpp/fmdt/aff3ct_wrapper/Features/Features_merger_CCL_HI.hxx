#pragma once

#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI.hpp"

uint32_t** Features_merger_CCL_HI::get_out_labels() {
    return this->out_labels;
}

aff3ct::runtime::Task& Features_merger_CCL_HI::operator[](const ftr_mrg::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Features_merger_CCL_HI::operator[](const ftr_mrg::sck::merge_v2 s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_mrg::tsk::merge_v2)[(size_t)s];
}

aff3ct::runtime::Socket& Features_merger_CCL_HI::operator[](const ftr_mrg::sck::merge_v3 s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_mrg::tsk::merge_v3)[(size_t)s];
}
