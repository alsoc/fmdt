#pragma once

#include "fmdt/aff3ct_wrapper/CCL/CCL.hpp"

uint32_t** CCL::get_out_labels() {
    return this->out_labels;
}

aff3ct::runtime::Task& CCL::operator[](const ccl::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& CCL::operator[](const ccl::sck::apply s) {
    return aff3ct::module::Module::operator[]((size_t)ccl::tsk::apply)[(size_t)s];
}
