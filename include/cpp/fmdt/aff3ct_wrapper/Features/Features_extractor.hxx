#pragma once

#include "fmdt/aff3ct_wrapper/Features/Features_extractor.hpp"

aff3ct::runtime::Task& Features_extractor::operator[](const ftr_ext::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Features_extractor::operator[](const ftr_ext::sck::extract s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_ext::tsk::extract)[(size_t)s];
}

aff3ct::runtime::Socket& Features_extractor::operator[](const std::string &tsk_sck) {
    return aff3ct::module::Module::operator[](tsk_sck);
}
