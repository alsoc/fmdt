#pragma once

#include "fmdt/Logger/Logger_frame.hpp"

aff3ct::runtime::Task& Logger_frame::operator[](const lgr_fra::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Logger_frame::operator[](const lgr_fra::sck::write s) {
    return aff3ct::module::Module::operator[]((size_t)lgr_fra::tsk::write)[(size_t)s];
}
