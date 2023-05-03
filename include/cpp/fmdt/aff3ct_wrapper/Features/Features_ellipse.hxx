#pragma once

#include "fmdt/aff3ct_wrapper/Features/Features_ellipse.hpp"

aff3ct::runtime::Task& Features_ellipse::operator[](const ftr_ell::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Features_ellipse::operator[](const ftr_ell::sck::compute s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_ell::tsk::compute)[(size_t)s];
}
