#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

namespace lgr_mtn {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_first_theta, in_first_tx, in_first_ty, in_first_mean_error,
                                    in_first_std_deviation, in_theta, in_tx, in_ty, in_mean_error, in_std_deviation,
                                    in_frame, status };
    }
}

class Logger_motion : public aff3ct::module::Module {
protected:
    const std::string motion_path;
public:
    Logger_motion(const std::string motion_path);
    virtual ~Logger_motion();

    inline aff3ct::module::Task& operator[](const lgr_mtn::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const lgr_mtn::sck::write s) {
        return aff3ct::module::Module::operator[]((size_t)lgr_mtn::tsk::write)[(size_t)s];
    }
};
