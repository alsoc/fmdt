/*!
 * \file
 * \brief C++ wrapper to log motion compensation.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

namespace lgr_mtn {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_motion_est1, in_motion_est2, in_frame, status };
    }
}

class Logger_motion : public spu::module::Stateful {
protected:
    const std::string motion_path;
    const size_t fra_start;
public:
    Logger_motion(const std::string motion_path, const size_t fra_start);
    virtual ~Logger_motion();
    inline spu::runtime::Task& operator[](const lgr_mtn::tsk t);
    inline spu::runtime::Socket& operator[](const lgr_mtn::sck::write s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Logger/Logger_motion.hxx"
