#pragma once

#include "fmdt/aff3ct_wrapper/Features/Features_motion.hpp"

aff3ct::runtime::Task& Features_motion::operator[](const ftr_mtn::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Features_motion::operator[](const ftr_mtn::sck::compute s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_mtn::tsk::compute)[(size_t)s];
}
