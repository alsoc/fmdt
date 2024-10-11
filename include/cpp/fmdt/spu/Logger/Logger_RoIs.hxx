#pragma once

#include "fmdt/spu/Logger/Logger_RoIs.hpp"

spu::runtime::Task& Logger_RoIs::operator[](const lgr_roi::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Logger_RoIs::operator[](const lgr_roi::sck::write s) {
    return spu::module::Module::operator[]((size_t)lgr_roi::tsk::write)[(size_t)s];
}

spu::runtime::Socket& Logger_RoIs::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
