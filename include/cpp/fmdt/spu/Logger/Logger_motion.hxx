#pragma once

#include "fmdt/spu/Logger/Logger_motion.hpp"

spu::runtime::Task& Logger_motion::operator[](const lgr_mtn::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Logger_motion::operator[](const lgr_mtn::sck::write s) {
    return spu::module::Module::operator[]((size_t)lgr_mtn::tsk::write)[(size_t)s];
}

spu::runtime::Socket& Logger_motion::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
