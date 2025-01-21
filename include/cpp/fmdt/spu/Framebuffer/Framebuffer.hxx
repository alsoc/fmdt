#pragma once

#include "fmdt/spu/Framebuffer/Framebuffer.hpp"

spu::runtime::Task& Framebuffer::operator[](const fb::tsk t) {
    return spu::module::Module::operator[]((size_t)t);
}

spu::runtime::Socket& Framebuffer::operator[](const fb::sck::bufferize s) {
    return spu::module::Module::operator[]((size_t)fb::tsk::bufferize)[(size_t)s];
}

spu::runtime::Socket& Framebuffer::operator[](const std::string &tsk_sck) {
    return spu::module::Module::operator[](tsk_sck);
}
