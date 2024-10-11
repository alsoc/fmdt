#pragma once

#include "fmdt/spu/Tracking/Tracking.hpp"

tracking_data_t* Tracking::get_data() {
    return this->tracking_data;
}

spu::runtime::Task& Tracking::operator[](const trk::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Tracking::operator[](const trk::sck::perform s) {
    return spu::module::Module::operator[]((size_t)trk::tsk::perform)[(size_t)s];
}

spu::runtime::Socket& Tracking::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
