/*!
 * \file
 * \brief C++ wrapper to compute ellipse features.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

namespace ftr_ell {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_RoIs_basic, in_n_RoIs, out_RoIs_elli, status };
    }
}

class Features_ellipse : public spu::module::Stateful {
protected:
    const size_t max_RoIs_size;
public:
    Features_ellipse(const size_t max_RoIs_size);
    virtual ~Features_ellipse() = default;
    virtual Features_ellipse* clone() const;
    inline spu::runtime::Task& operator[](const ftr_ell::tsk t);
    inline spu::runtime::Socket& operator[](const ftr_ell::sck::compute s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Features/Features_ellipse.hxx"
