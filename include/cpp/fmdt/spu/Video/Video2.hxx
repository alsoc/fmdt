#pragma once

#include "fmdt/spu/Video/Video2.hpp"

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

void Video2::set_loop_size(size_t loop_size) {
    this->video->loop_size = loop_size;
}

spu::runtime::Task& Video2::operator[](const vid2::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Video2::operator[](const vid2::sck::generate s) {
    return spu::module::Module::operator[]((size_t)vid2::tsk::generate)[(size_t)s];
}

spu::runtime::Socket& Video2::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
