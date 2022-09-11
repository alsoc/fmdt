#pragma once

#include "fmdt/Tracking/Tracking.hpp"

track_t* Tracking::get_track_array() {
    return this->track_array;
}

BB_t** Tracking::get_BB_array() {
    return this->BB_array;
}

aff3ct::module::Task& Tracking::operator[](const trk::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::module::Socket& Tracking::operator[](const trk::sck::perform s) {
    return aff3ct::module::Module::operator[]((size_t)trk::tsk::perform)[(size_t)s];
}
