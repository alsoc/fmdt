#pragma once

#include "fmdt/Logger/Logger_motion.hpp"

aff3ct::module::Task& Logger_motion::operator[](const lgr_mtn::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::module::Socket& Logger_motion::operator[](const lgr_mtn::sck::write s) {
    return aff3ct::module::Module::operator[]((size_t)lgr_mtn::tsk::write)[(size_t)s];
}
