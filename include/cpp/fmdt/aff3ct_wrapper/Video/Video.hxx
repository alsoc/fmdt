#pragma once

#include "fmdt/aff3ct_wrapper/Video/Video.hpp"

uint8_t** Video::get_out_img() {
    return this->out_img;
}

int Video::get_i0() {
    return this->i0;
}

int Video::get_i1() {
    return this->i1;
}

int Video::get_j0() {
    return this->j0;
}

int Video::get_j1() {
    return this->j1;
}

int Video::get_b() {
    return this->b;
}

void Video::set_loop_size(size_t loop_size) {
    this->video->loop_size = loop_size;
}

aff3ct::runtime::Task& Video::operator[](const vid::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Video::operator[](const vid::sck::generate s) {
    return aff3ct::module::Module::operator[]((size_t)vid::tsk::generate)[(size_t)s];
}
