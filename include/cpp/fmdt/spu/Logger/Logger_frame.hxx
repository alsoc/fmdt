#pragma once

#include "fmdt/spu/Logger/Logger_frame.hpp"

spu::runtime::Task& Logger_frame::operator[](const lgr_fra::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Logger_frame::operator[](const lgr_fra::sck::write s) {
    return spu::module::Module::operator[]((size_t)lgr_fra::tsk::write)[(size_t)s];
}

spu::runtime::Socket& Logger_frame::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
