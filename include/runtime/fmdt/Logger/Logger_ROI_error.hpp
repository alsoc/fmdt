#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace lgr_err {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_ROI_id, in_ROI_dx, in_ROI_dy, in_ROI_error, in_ROI_next_id, in_ROI_is_moving,
                                    in_n_ROI, in_frame, status };
    }
}

class Logger_ROI_error : public aff3ct::module::Module {
protected:
    const std::string ROI_error_path;
public:
    Logger_ROI_error(const std::string ROI_error_path, const size_t max_ROI_size);
    virtual ~Logger_ROI_error();
    inline aff3ct::runtime::Task& operator[](const lgr_err::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_err::sck::write s);
};

#include "fmdt/Logger/Logger_ROI_error.hxx"
