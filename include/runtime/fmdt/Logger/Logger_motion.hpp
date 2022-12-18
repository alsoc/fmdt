#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace lgr_mtn {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_motion_est1, in_motion_est2, in_frame, status };
    }
}

class Logger_motion : public aff3ct::module::Module {
protected:
    const std::string motion_path;
public:
    Logger_motion(const std::string motion_path);
    virtual ~Logger_motion();
    inline aff3ct::runtime::Task& operator[](const lgr_mtn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_mtn::sck::write s);
};

#include "fmdt/Logger/Logger_motion.hxx"
