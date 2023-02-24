/*!
 * \file
 * \brief C++ wrapper for the CCL algorithms.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/CCL.h"

namespace ccl_tf {
    enum class tsk : size_t { apply, SIZE };
    namespace sck {
        enum class apply : size_t { in_img, out_labels, out_n_RoIs, out_RoIs_id, out_RoIs_xmin, out_RoIs_xmax,
                                    out_RoIs_ymin, out_RoIs_ymax, out_RoIs_S, out_RoIs_Sx, out_RoIs_Sy, out_RoIs_x,
                                    out_RoIs_y, status };
    }
}

class CCL_threshold_features : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const uint8_t threshold;
    const size_t max_RoIs_size;
    CCL_gen_data_t *data;
    const uint8_t** in_img;
    uint32_t** out_labels;

public:
    CCL_threshold_features(const int i0, const int i1, const int j0, const int j1, const int b, const uint8_t threshold,
                           const size_t max_RoIs_size, const enum ccl_impl_e impl = LSLH);
    virtual ~CCL_threshold_features();
    virtual CCL_threshold_features* clone() const;
    inline uint32_t** get_out_labels();
    inline aff3ct::runtime::Task& operator[](const ccl_tf::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ccl_tf::sck::apply s);

protected:
    void init_data(const enum ccl_impl_e impl);
    using Module::deep_copy;
    void deep_copy(const CCL_threshold_features &m);
};

#include "fmdt/aff3ct_wrapper/CCL/CCL_threshold_features.hxx"
