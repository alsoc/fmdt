#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace mtn {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_ROI0_x, in_ROI0_y, in_ROI1_x, in_ROI1_y, in_ROI1_prev_id, in_n_ROI1,
                                      out_ROI1_dx, out_ROI1_dy, out_ROI1_error, out_ROI1_is_moving, out_motion_est1,
                                      out_motion_est2, status };
    }
}

class Motion : public aff3ct::module::Module {
protected:
    const size_t max_ROI_size;
public:
    Motion(const size_t max_ROI_size);
    virtual ~Motion();
    inline aff3ct::runtime::Task& operator[](const mtn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const mtn::sck::compute s);
};

#include "fmdt/aff3ct_wrapper/Motion/Motion.hxx"
