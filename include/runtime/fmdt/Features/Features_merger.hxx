#pragma once

#include "fmdt/Features/Features_merger.hpp"

uint8_t** Features_merger::get_out_img() {
    return this->out_img;
}

aff3ct::runtime::Task& Features_merger::operator[](const ftr_mrg::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Features_merger::operator[](const ftr_mrg::sck::merge s) {
    return aff3ct::module::Module::operator[]((size_t)ftr_mrg::tsk::merge)[(size_t)s];
}
