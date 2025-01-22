/*!
 * \file
 * \brief C++ wrapper to image transformations.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

namespace imgc {
    enum class tsk : size_t { gray2rgb, SIZE };
    namespace sck {
        enum class gray2rgb : size_t { fwd_img_gray8, out_img_rgb24, status };
    }
}

class Image_compute : public spu::module::Stateful {
protected:
  const size_t i0;
  const size_t i1;
  const size_t j0;
  const size_t j1;
  const size_t border;
public:
    Image_compute(const size_t i0, const size_t i1, const size_t j0, const size_t j1, const size_t border);
    virtual ~Image_compute() = default;
    virtual Image_compute* clone() const;

    inline spu::runtime::Task& operator[](const imgc::tsk t);
    inline spu::runtime::Socket& operator[](const imgc::sck::gray2rgb s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Image_compute/Image_compute.hxx"
