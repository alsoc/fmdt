/*!
 * \file
 * \brief C++ wrapper to compute image threshold.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

namespace thr {
    enum class tsk : size_t { apply, SIZE };
    namespace sck {
        enum class apply : size_t { in_img, out_img, status };
    }
}

class Threshold : public spu::module::Stateful {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const uint8_t thr_val;
public:
    Threshold(const int i0, const int i1, const int j0, const int j1, const int b, const uint8_t thr_val);
    virtual ~Threshold();
    virtual Threshold* clone() const;
    inline uint8_t** get_out_img();
    inline spu::runtime::Task& operator[](const thr::tsk t);
    inline spu::runtime::Socket& operator[](const thr::sck::apply s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Threshold/Threshold.hxx"
