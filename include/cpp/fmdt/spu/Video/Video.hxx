#pragma once

#include "fmdt/spu/Video/Video.hpp"

uint8_t** Video::get_out_img() {
    return (*this)[vid::sck::generate::out_img].get_2d_dataptr<uint8_t>(this->b, this->b);
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

spu::runtime::Task& Video::operator[](const vid::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Video::operator[](const vid::sck::generate s) {
    return spu::module::Module::operator[]((size_t)vid::tsk::generate)[(size_t)s];
}

spu::runtime::Socket& Video::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
