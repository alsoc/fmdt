#pragma once

#include "fmdt/Logger/Logger_KNN.hpp"

aff3ct::module::Task& Logger_KNN::operator[](const lgr_knn::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::module::Socket& Logger_KNN::operator[](const lgr_knn::sck::write s) {
    return aff3ct::module::Module::operator[]((size_t)lgr_knn::tsk::write)[(size_t)s];
}
