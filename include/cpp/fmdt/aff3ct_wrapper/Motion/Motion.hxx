#pragma once

#include "fmdt/aff3ct_wrapper/Motion/Motion.hpp"

aff3ct::runtime::Task& Motion::operator[](const mtn::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Motion::operator[](const mtn::sck::compute s) {
    return aff3ct::module::Module::operator[]((size_t)mtn::tsk::compute)[(size_t)s];
}
