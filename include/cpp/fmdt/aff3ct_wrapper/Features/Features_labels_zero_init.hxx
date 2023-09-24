#pragma once

#include "fmdt/aff3ct_wrapper/Features/Features_labels_zero_init.hpp"

aff3ct::runtime::Task& Features_labels_zero_init::operator[](const ftr_lzi::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Features_labels_zero_init::operator[](const ftr_lzi::sck::zinit s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_lzi::tsk::zinit)[(size_t)s];
}

aff3ct::runtime::Socket& Features_labels_zero_init::operator[](const std::string &tsk_sck) {
    return aff3ct::module::Module::operator[](tsk_sck);
}
