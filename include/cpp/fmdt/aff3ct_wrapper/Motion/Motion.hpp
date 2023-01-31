#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace mtn {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_RoI0_x, in_RoI0_y, in_RoI1_x, in_RoI1_y, in_RoI1_prev_id, in_n_RoI1,
                                      out_RoI1_dx, out_RoI1_dy, out_RoI1_error, out_RoI1_is_moving, out_motion_est1,
                                      out_motion_est2, status };
    }
}

class Motion : public aff3ct::module::Module {
protected:
    const size_t max_RoI_size;
public:
    Motion(const size_t max_RoI_size);
    virtual ~Motion();
    inline aff3ct::runtime::Task& operator[](const mtn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const mtn::sck::compute s);
};

#include "fmdt/aff3ct_wrapper/Motion/Motion.hxx"
