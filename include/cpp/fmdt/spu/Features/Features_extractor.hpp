/*!
 * \file
 * \brief C++ wrapper for the features extraction.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

namespace ftr_ext {
    enum class tsk : size_t { extract, SIZE };
    namespace sck {
        enum class extract : size_t { in_labels, in_n_RoIs, out_RoIs_basic, status };
    }
}

class Features_extractor : public spu::module::Stateful {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const size_t max_RoIs_size;
public:
    Features_extractor(const int i0, const int i1, const int j0, const int j1, const int b, const size_t max_RoIs_size);
    virtual ~Features_extractor();
    virtual Features_extractor* clone() const;
    inline spu::runtime::Task& operator[](const ftr_ext::tsk t);
    inline spu::runtime::Socket& operator[](const ftr_ext::sck::extract s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Features/Features_extractor.hxx"
