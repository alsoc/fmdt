#pragma once

#include "fmdt/KNN_matcher/KNN_matcher.hpp"

aff3ct::module::Task& KNN_matcher::operator[](const knn::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::module::Socket& KNN_matcher::operator[](const knn::sck::match s) {
    return aff3ct::module::Module::operator[]((size_t)knn::tsk::match)[(size_t)s];
}
