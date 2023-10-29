#pragma once

#include "fmdt/aff3ct_wrapper/CCL/CCL.hpp"

uint32_t** CCL::get_out_labels() {
    return (*this)[ccl::sck::apply::out_labels].get_2d_dataptr<uint32_t>(this->b, this->b);
}

aff3ct::runtime::Task& CCL::operator[](const ccl::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& CCL::operator[](const ccl::sck::apply s) {
    return aff3ct::module::Module::operator[]((size_t)ccl::tsk::apply)[(size_t)s];
}

aff3ct::runtime::Socket& CCL::operator[](const std::string &tsk_sck) {
    return aff3ct::module::Module::operator[](tsk_sck);
}
