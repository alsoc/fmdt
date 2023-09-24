#pragma once

#include "fmdt/aff3ct_wrapper/kNN_matcher/kNN_matcher.hpp"

aff3ct::runtime::Task& kNN_matcher::operator[](const knn::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& kNN_matcher::operator[](const knn::sck::match s) {
    return aff3ct::module::Module::operator[]((size_t)knn::tsk::match)[(size_t)s];
}

aff3ct::runtime::Socket& kNN_matcher::operator[](const std::string &tsk_sck) {
    return aff3ct::module::Module::operator[](tsk_sck);
}
