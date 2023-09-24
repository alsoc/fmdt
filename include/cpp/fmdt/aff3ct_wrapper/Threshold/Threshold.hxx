#pragma once

#include "fmdt/aff3ct_wrapper/Threshold/Threshold.hpp"

uint8_t** Threshold::get_out_img() {
    return this->out_img;
}

aff3ct::runtime::Task& Threshold::operator[](const thr::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Threshold::operator[](const thr::sck::apply s) {
    return aff3ct::module::Module::operator[]((size_t)thr::tsk::apply)[(size_t)s];
}

aff3ct::runtime::Socket& Threshold::operator[](const std::string &tsk_sck) {
    return aff3ct::module::Module::operator[](tsk_sck);
}
