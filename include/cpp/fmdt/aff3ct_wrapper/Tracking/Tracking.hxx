#pragma once

#include "fmdt/aff3ct_wrapper/Tracking/Tracking.hpp"

tracking_data_t* Tracking::get_data() {
    return this->tracking_data;
}

vec_BB_t* Tracking::get_BB_array() {
    return this->BB_array;
}

aff3ct::runtime::Task& Tracking::operator[](const trk::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Tracking::operator[](const trk::sck::perform s) {
    return aff3ct::module::Module::operator[]((size_t)trk::tsk::perform)[(size_t)s];
}
