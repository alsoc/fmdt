/*!
 * \file
 * \brief C++ wrapper to log motion compensation.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace lgr_mtn {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_motion_est1, in_motion_est2, in_frame, status };
    }
}
#endif

class Logger_motion : public aff3ct::module::Module {
protected:
    const std::string motion_path;
    const size_t fra_start;
public:
    Logger_motion(const std::string motion_path, const size_t fra_start);
    virtual ~Logger_motion();
    inline aff3ct::runtime::Task& operator[](const lgr_mtn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_mtn::sck::write s);
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "fmdt/aff3ct_wrapper/Logger/Logger_motion.hxx"
#endif
