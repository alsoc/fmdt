/*!
 * \file
 * \brief C++ wrapper for labels zero init.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

namespace ftr_lzi {
    enum class tsk : size_t { zinit, SIZE };
    namespace sck {
        enum class zinit : size_t { in_RoIs_basic, in_n_RoIs, fwd_labels, status };
    }
}

class Features_labels_zero_init : public spu::module::Stateful {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const size_t max_in_RoIs_size;
public:
    Features_labels_zero_init(const int i0, const int i1, const int j0, const int j1, const int b,
                              const size_t max_in_RoIs_size);
    virtual ~Features_labels_zero_init();
    virtual Features_labels_zero_init* clone() const;

    inline spu::runtime::Task& operator[](const ftr_lzi::tsk t);
    inline spu::runtime::Socket& operator[](const ftr_lzi::sck::zinit s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Features/Features_labels_zero_init.hxx"
