#pragma once

#include "fmdt/aff3ct_wrapper/Visu/Visu.hpp"

aff3ct::runtime::Task& Visu::operator[](const vis::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Visu::operator[](const vis::sck::display s) {
    return aff3ct::module::Module::operator[]((size_t)vis::tsk::display)[(size_t)s];
}
