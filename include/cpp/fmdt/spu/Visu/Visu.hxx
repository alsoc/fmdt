#pragma once

#include "fmdt/spu/Visu/Visu.hpp"

spu::runtime::Task& Visu::operator[](const vis::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Visu::operator[](const vis::sck::display s) {
    return spu::module::Module::operator[]((size_t)vis::tsk::display)[(size_t)s];
}

spu::runtime::Socket& Visu::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
