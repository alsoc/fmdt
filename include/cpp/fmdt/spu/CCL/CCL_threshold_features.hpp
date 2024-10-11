/*!
 * \file
 * \brief C++ wrapper for the CCL algorithms (+ threshold & basic features).
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

#include "fmdt/CCL.h"

namespace ccl_tf {
    enum class tsk : size_t { apply, SIZE };
    namespace sck {
        enum class apply : size_t { in_img, out_labels, out_RoIs_basic, out_n_RoIs, status };
    }
}

class CCL_threshold_features : public spu::module::Stateful {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const uint8_t threshold;
    const size_t max_RoIs_size;
    CCL_gen_data_t *data;
    const bool no_init_labels;

public:
    CCL_threshold_features(const int i0, const int i1, const int j0, const int j1, const int b, const uint8_t threshold,
                           const size_t max_RoIs_size, const enum ccl_impl_e impl = LSLH,
                           const bool no_init_labels = false);
    virtual ~CCL_threshold_features();
    virtual CCL_threshold_features* clone() const;
    inline uint32_t** get_out_labels();
    inline spu::runtime::Task& operator[](const ccl_tf::tsk t);
    inline spu::runtime::Socket& operator[](const ccl_tf::sck::apply s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);

protected:
    void init_data(const enum ccl_impl_e impl);
    using spu::module::Stateful::deep_copy;
    void deep_copy(const CCL_threshold_features &m);
};

#include "fmdt/spu/CCL/CCL_threshold_features.hxx"
