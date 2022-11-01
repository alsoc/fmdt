#pragma once

#include "fmdt/Video/Video2.hpp"

uint8_t** Video2::get_out_img0() {
    return this->out_img0;
}

uint8_t** Video2::get_img_buf() {
    return this->img_buf;
}

int Video2::get_i0() {
    return this->i0;
}

int Video2::get_i1() {
    return this->i1;
}

int Video2::get_j0() {
    return this->j0;
}

int Video2::get_j1() {
    return this->j1;
}

int Video2::get_b() {
    return this->b;
}

aff3ct::runtime::Task& Video2::operator[](const vid2::tsk t) {
    return aff3ct::module::Module::operator[]((size_t)t);
}

aff3ct::runtime::Socket& Video2::operator[](const vid2::sck::generate s) {
    return aff3ct::module::Module::operator[]((size_t)vid2::tsk::generate)[(size_t)s];
}
