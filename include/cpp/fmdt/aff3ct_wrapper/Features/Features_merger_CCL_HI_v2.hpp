/*!
 * \file
 * \brief C++ wrapper to compute/merge CCL with hysteresis.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ftr_mrg2 {
    enum class tsk : size_t { merge, SIZE };
    namespace sck {
        enum class merge : size_t { in_labels, in_img_HI, fwd_RoIs_basic, in_n_RoIs, out_RoIs_basic, out_n_RoIs,
                                    out_labels, status };
    }
}

class Features_merger_CCL_HI_v2 : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const int S_min, S_max;
    const size_t max_in_RoIs_size;
    const size_t max_out_RoIs_size;
public:
    Features_merger_CCL_HI_v2(const int i0, const int i1, const int j0, const int j1, const int b, const uint32_t S_min,
                              const uint32_t S_max, const size_t max_in_RoIs_size, const size_t max_out_RoIs_size);
    virtual ~Features_merger_CCL_HI_v2();
    virtual Features_merger_CCL_HI_v2* clone() const;
    inline uint32_t** get_out_labels();
    inline aff3ct::runtime::Task& operator[](const ftr_mrg2::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ftr_mrg2::sck::merge s);
    inline aff3ct::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI_v2.hxx"
