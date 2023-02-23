#pragma once

#include "fmdt/aff3ct_wrapper/CCL_LSL/CCL_threshold_features.hpp"

uint32_t** CCL_threshold_features::get_out_labels() {
    return this->out_labels;
}

aff3ct::runtime::Task& CCL_threshold_features::operator[](const ccl_tf::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& CCL_threshold_features::operator[](const ccl_tf::sck::apply s) {
    return aff3ct::module::Module::operator[]((size_t)ccl_tf::tsk::apply)[(size_t)s];
}
