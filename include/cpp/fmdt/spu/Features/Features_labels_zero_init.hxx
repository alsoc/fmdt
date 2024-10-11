#pragma once

#include "fmdt/spu/Features/Features_labels_zero_init.hpp"

spu::runtime::Task& Features_labels_zero_init::operator[](const ftr_lzi::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Features_labels_zero_init::operator[](const ftr_lzi::sck::zinit s) {
    return spu::module::Module::operator[]((size_t)ftr_lzi::tsk::zinit)[(size_t)s];
}

spu::runtime::Socket& Features_labels_zero_init::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
