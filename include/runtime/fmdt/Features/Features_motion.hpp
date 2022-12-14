#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ftr_mtn {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_ROI0_x, in_ROI0_y, in_ROI1_x, in_ROI1_y, in_ROI1_prev_id, in_n_ROI1,
                                      out_ROI1_dx, out_ROI1_dy, out_ROI1_error, out_ROI1_is_moving, out_first_theta,
                                      out_first_tx, out_first_ty, out_first_mean_error, out_first_std_deviation,
                                      out_theta, out_tx, out_ty, out_mean_error, out_std_deviation, status };
    }
}

class Features_motion : public aff3ct::module::Module {
protected:
    const size_t max_ROI_size;
public:
    Features_motion(const size_t max_ROI_size);
    virtual ~Features_motion();
    inline aff3ct::runtime::Task& operator[](const ftr_mtn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ftr_mtn::sck::compute s);
};

#include "fmdt/Features/Features_motion.hxx"
