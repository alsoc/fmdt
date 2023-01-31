#pragma once

#include "fmdt/aff3ct_wrapper/Logger/Logger_tracks.hpp"

aff3ct::runtime::Task& Logger_tracks::operator[](const lgr_trk::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Logger_tracks::operator[](const lgr_trk::sck::write s) {
    return aff3ct::module::Module::operator[]((size_t)lgr_trk::tsk::write)[(size_t)s];
}
