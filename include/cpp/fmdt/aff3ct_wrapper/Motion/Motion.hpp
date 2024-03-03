/*!
 * \file
 * \brief C++ wrapper to compute motion for each RoI.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace mtn {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_RoIs0_basic, in_RoIs1_basic, in_RoIs1_asso, in_n_RoIs1, out_RoIs1_motion,
                                      out_motion_est1, out_motion_est2, status };
    }
}

class Motion : public aff3ct::module::Module {
protected:
    const size_t max_RoIs_size;
public:
    Motion(const size_t max_RoIs_size);
    virtual ~Motion();
    virtual Motion* clone() const;
    inline aff3ct::runtime::Task& operator[](const mtn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const mtn::sck::compute s);
    inline aff3ct::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/aff3ct_wrapper/Motion/Motion.hxx"
