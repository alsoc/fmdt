#pragma once

#include "fmdt/Images/Images.hpp"

uint8_t** Images::get_out_img() {
    return this->out_img;
}

int Images::get_i0() {
    return this->images->i0;
}

int Images::get_i1() {
    return this->images->i1;
}

int Images::get_j0() {
    return this->images->j0;
}

int Images::get_j1() {
    return this->images->j1;
}

int Images::get_b() {
    return this->b;
}

void Images::set_loop_size(size_t loop_size) {
    this->images->loop_size = loop_size;
}

aff3ct::runtime::Task& Images::operator[](const img::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Images::operator[](const img::sck::generate s) {
    return aff3ct::module::Module::operator[]((size_t)img::tsk::generate)[(size_t)s];
}
