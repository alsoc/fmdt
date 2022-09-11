#pragma once

#include "fmdt/Logger/Logger_ROI.hpp"

aff3ct::module::Task& Logger_ROI::operator[](const lgr_roi::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::module::Socket& Logger_ROI::operator[](const lgr_roi::sck::write s) {
    return aff3ct::module::Module::operator[]((size_t)lgr_roi::tsk::write)[(size_t)s];
}
