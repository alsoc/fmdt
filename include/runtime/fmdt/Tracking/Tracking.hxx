#pragma once

#include "fmdt/Tracking/Tracking.hpp"

track_t* Tracking::get_track_array() {
    return this->track_array;
}

BB_t** Tracking::get_BB_array() {
    return this->BB_array;
}

int32_t* Tracking::get_ROI0_prev_id() {
    return this->ROI0_time;
}

int32_t* Tracking::get_ROI0_time() {
    return this->ROI0_time;
}

int32_t* Tracking::get_ROI0_time_motion() {
    return this->ROI0_time_motion;
}

uint8_t* Tracking::get_ROI0_is_extrapolated() {
    return this->ROI0_is_extrapolated;
}

int32_t* Tracking::get_ROI1_time() {
    return this->ROI1_time;
}

int32_t* Tracking::get_ROI1_time_motion() {
    return this->ROI1_time_motion;
}

uint8_t* Tracking::get_ROI1_is_extrapolated() {
    return this->ROI1_is_extrapolated;
}

aff3ct::runtime::Task& Tracking::operator[](const trk::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Tracking::operator[](const trk::sck::perform s) {
    return aff3ct::module::Module::operator[]((size_t)trk::tsk::perform)[(size_t)s];
}
