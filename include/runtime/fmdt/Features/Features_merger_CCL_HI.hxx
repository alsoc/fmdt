#pragma once

#include "fmdt/Features/Features_merger_CCL_HI.hpp"

uint32_t** Features_merger_CCL_HI::get_out_labels() {
    return this->out_labels;
}

aff3ct::runtime::Task& Features_merger_CCL_HI::operator[](const ftr_mrg::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Features_merger_CCL_HI::operator[](const ftr_mrg::sck::merge s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_mrg::tsk::merge)[(size_t)s];
}
