#pragma once

#include "fmdt/aff3ct_wrapper/Logger/Logger_motion.hpp"

aff3ct::runtime::Task& Logger_motion::operator[](const lgr_mtn::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Logger_motion::operator[](const lgr_mtn::sck::write s) {
    return aff3ct::module::Module::operator[]((size_t)lgr_mtn::tsk::write)[(size_t)s];
}

aff3ct::runtime::Socket& Logger_motion::operator[](const std::string &tsk_sck) {
    return aff3ct::module::Module::operator[](tsk_sck);
}
