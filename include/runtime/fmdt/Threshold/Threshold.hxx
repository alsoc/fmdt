#pragma once

#include "fmdt/Threshold/Threshold.hpp"

uint8_t** Threshold::get_out_img() {
    return this->out_img;
}

aff3ct::module::Task& Threshold::operator[](const thr::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::module::Socket& Threshold::operator[](const thr::sck::apply s) {
    return aff3ct::module::Module::operator[]((size_t)thr::tsk::apply)[(size_t)s];
}
