#pragma once

#include "fmdt/Logger/Logger_ROI_error.hpp"

aff3ct::runtime::Task& Logger_ROI_error::operator[](const lgr_err::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Logger_ROI_error::operator[](const lgr_err::sck::write s) {
    return aff3ct::module::Module::operator[]((size_t)lgr_err::tsk::write)[(size_t)s];
}
