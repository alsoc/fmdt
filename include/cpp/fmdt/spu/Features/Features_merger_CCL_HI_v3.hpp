/*!
 * \file
 * \brief C++ wrapper to compute/merge CCL with hysteresis (optimized version).
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

namespace ftr_mrg3 {
    enum class tsk : size_t { merge, SIZE };
    namespace sck {
        enum class merge : size_t { in_labels, in_img, fwd_RoIs_basic, in_n_RoIs, out_RoIs_basic, out_n_RoIs,
                                    out_labels, status };
    }
}

class Features_merger_CCL_HI_v3 : public spu::module::Stateful {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const int S_min, S_max;
    const uint8_t threshold_high, no_labels_zeros_init;
    const size_t max_in_RoIs_size;
    const size_t max_out_RoIs_size;
public:
    Features_merger_CCL_HI_v3(const int i0, const int i1, const int j0, const int j1, const int b, const uint32_t S_min,
                              const uint32_t S_max, const uint8_t threshold_high, const uint8_t no_labels_zeros_init,
                              const size_t max_in_RoIs_size, const size_t max_out_RoIs_size);
    virtual ~Features_merger_CCL_HI_v3();
    virtual Features_merger_CCL_HI_v3* clone() const;
    inline uint32_t** get_out_labels();
    inline spu::runtime::Task& operator[](const ftr_mrg3::tsk t);
    inline spu::runtime::Socket& operator[](const ftr_mrg3::sck::merge s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Features/Features_merger_CCL_HI_v3.hxx"
