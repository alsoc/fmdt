#pragma once

#include "fmdt/CCL_LSL/CCL_LSL.hpp"

uint32_t** CCL_LSL::get_out_img() {
    return this->out_img;
}

aff3ct::module::Task& CCL_LSL::operator[](const ccl::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::module::Socket& CCL_LSL::operator[](const ccl::sck::apply s) {
    return aff3ct::module::Module::operator[]((size_t)ccl::tsk::apply)[(size_t)s];
}
