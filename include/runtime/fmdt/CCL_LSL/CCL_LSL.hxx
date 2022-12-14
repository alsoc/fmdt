#pragma once

#include "fmdt/CCL_LSL/CCL_LSL.hpp"

uint32_t** CCL_LSL::get_out_labels() {
    return this->out_labels;
}

aff3ct::runtime::Task& CCL_LSL::operator[](const ccl::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& CCL_LSL::operator[](const ccl::sck::apply s) {
    return aff3ct::module::Module::operator[]((size_t)ccl::tsk::apply)[(size_t)s];
}
