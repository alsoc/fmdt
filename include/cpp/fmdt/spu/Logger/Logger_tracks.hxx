#pragma once

#include "fmdt/spu/Logger/Logger_tracks.hpp"

spu::runtime::Task& Logger_tracks::operator[](const lgr_trk::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Logger_tracks::operator[](const lgr_trk::sck::write s) {
    return spu::module::Module::operator[]((size_t)lgr_trk::tsk::write)[(size_t)s];
}

spu::runtime::Socket& Logger_tracks::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
