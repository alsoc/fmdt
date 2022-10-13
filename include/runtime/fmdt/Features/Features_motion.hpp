#pragma once

#include <stdint.h>
#include <aff3ct_core.hpp>

namespace ftr_mtn {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_ROI0_next_id, in_ROI0_x, in_ROI0_y, in_n_ROI0, in_ROI1_x, in_ROI1_y,
                                      out_ROI0_dx, out_ROI0_dy, out_ROI0_error, out_ROI0_is_moving, out_first_theta,
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
    inline aff3ct::module::Task& operator[](const ftr_mtn::tsk t);
    inline aff3ct::module::Socket& operator[](const ftr_mtn::sck::compute s);
};

#include "fmdt/Features/Features_motion.hxx"
