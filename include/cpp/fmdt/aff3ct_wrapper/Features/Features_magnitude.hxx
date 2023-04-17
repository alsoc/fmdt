#pragma once

#include "fmdt/aff3ct_wrapper/Features/Features_magnitude.hpp"

aff3ct::runtime::Task& Features_magnitude::operator[](const ftr_mgn::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Features_magnitude::operator[](const ftr_mgn::sck::compute s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_mgn::tsk::compute)[(size_t)s];
}
