/*!
 * \file
 * \brief C++ wrapper to compute motion for each RoI.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace mtn {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_RoIs0_x, in_RoIs0_y, in_RoIs1_x, in_RoIs1_y, in_RoIs1_prev_id, in_n_RoIs1,
                                      out_RoIs1_dx, out_RoIs1_dy, out_RoIs1_error, out_RoIs1_is_moving, out_motion_est1,
                                      out_motion_est2, status };
    }
}
#endif

class Motion : public aff3ct::module::Module {
protected:
    const size_t max_RoIs_size;
public:
    Motion(const size_t max_RoIs_size);
    virtual ~Motion();
    inline aff3ct::runtime::Task& operator[](const mtn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const mtn::sck::compute s);
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "fmdt/aff3ct_wrapper/Motion/Motion.hxx"
#endif
