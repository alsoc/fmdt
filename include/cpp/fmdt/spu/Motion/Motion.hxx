#pragma once

#include "fmdt/spu/Motion/Motion.hpp"

spu::runtime::Task& Motion::operator[](const mtn::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Motion::operator[](const mtn::sck::compute s) {
    return spu::module::Module::operator[]((size_t)mtn::tsk::compute)[(size_t)s];
}

spu::runtime::Socket& Motion::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}