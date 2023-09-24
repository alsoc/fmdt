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
        enum class merge : size_t { in_labels, in_img_HI, in_RoIs_id, in_RoIs_xmin, in_RoIs_xmax, in_RoIs_ymin,
                                    in_RoIs_ymax, in_RoIs_S, in_RoIs_Sx, in_RoIs_Sy, in_RoIs_Sx2, in_RoIs_Sy2,
                                    in_RoIs_Sxy, in_RoIs_x, in_RoIs_y, in_n_RoIs,
                                    out_RoIs_id, out_RoIs_xmin, out_RoIs_xmax, out_RoIs_ymin, out_RoIs_ymax,
                                    out_RoIs_S, out_RoIs_Sx, out_RoIs_Sy, out_RoIs_Sx2, out_RoIs_Sy2, out_RoIs_Sxy,
                                    out_RoIs_x, out_RoIs_y, out_n_RoIs, out_labels, status };
    }
}

class Features_merger_CCL_HI_v2 : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const int S_min, S_max;
    const size_t max_in_RoIs_size;
    const size_t max_out_RoIs_size;
    const uint32_t** in_labels;
    const uint8_t** in_img_HI;
    uint32_t** out_labels;
    uint32_t* tmp_in_RoIs_id;
public:
    Features_merger_CCL_HI_v2(const int i0, const int i1, const int j0, const int j1, const int b, const uint32_t S_min,
                              const uint32_t S_max, const size_t max_in_RoIs_size, const size_t max_out_RoIs_size);
    virtual ~Features_merger_CCL_HI_v2();
    virtual Features_merger_CCL_HI_v2* clone() const;
    inline uint32_t** get_out_labels();
    inline aff3ct::runtime::Task& operator[](const ftr_mrg2::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ftr_mrg2::sck::merge s);
    inline aff3ct::runtime::Socket& operator[](const std::string &tsk_sck);

protected:
    void init_data();
    using Module::deep_copy;
    void deep_copy(const Features_merger_CCL_HI_v2 &m);
};

#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI_v2.hxx"
