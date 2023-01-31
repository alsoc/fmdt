#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ftr_mrg {
    enum class tsk : size_t { merge, SIZE };
    namespace sck {
        enum class merge : size_t { in_labels, in_img_HI, in_RoI_id, in_RoI_xmin, in_RoI_xmax, in_RoI_ymin,
                                    in_RoI_ymax, in_RoI_S, in_RoI_Sx, in_RoI_Sy, in_RoI_x, in_RoI_y, in_n_RoI,
                                    out_RoI_id, out_RoI_xmin, out_RoI_xmax, out_RoI_ymin, out_RoI_ymax, out_RoI_S,
                                    out_RoI_Sx, out_RoI_Sy, out_RoI_x, out_RoI_y, out_n_RoI, out_labels, status };
    }
}

class Features_merger_CCL_HI : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const int S_min, S_max;
    const size_t max_in_RoI_size;
    const size_t max_out_RoI_size;
    const uint32_t** in_labels;
    const uint8_t** in_img_HI;
    uint32_t** out_labels;
    uint32_t* tmp_in_RoI_id;
public:
    Features_merger_CCL_HI(const int i0, const int i1, const int j0, const int j1, const int b, const uint32_t S_min,
                           const uint32_t S_max, const size_t max_in_RoI_size, const size_t max_out_RoI_size);
    virtual ~Features_merger_CCL_HI();
    virtual Features_merger_CCL_HI* clone() const;
    inline uint32_t** get_out_labels();
    inline aff3ct::runtime::Task& operator[](const ftr_mrg::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ftr_mrg::sck::merge s);
protected:
    void init_data();
    using Module::deep_copy;
    void deep_copy(const Features_merger_CCL_HI &m);
};

#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI.hxx"
