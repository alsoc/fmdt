#pragma once

#include "fmdt/Logger/Logger_track.hpp"

aff3ct::module::Task& Logger_track::operator[](const lgr_trk::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::module::Socket& Logger_track::operator[](const lgr_trk::sck::write s) {
    return aff3ct::module::Module::operator[]((size_t)lgr_trk::tsk::write)[(size_t)s];
}
