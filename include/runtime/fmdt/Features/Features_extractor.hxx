#pragma once

#include "fmdt/Features/Features_extractor.hpp"

aff3ct::module::Task& Features_extractor::operator[](const ftr_ext::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::module::Socket& Features_extractor::operator[](const ftr_ext::sck::extract s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_ext::tsk::extract)[(size_t)s];
}
