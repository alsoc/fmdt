/*!
 * \file
 * \brief C++ wrapper to log RoIs features.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tracking/tracking_struct.h"

namespace lgr_roi {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_RoIs0_id, in_RoIs0_xmin, in_RoIs0_xmax, in_RoIs0_ymin, in_RoIs0_ymax,
                                    in_RoIs0_S, in_RoIs0_Sx, in_RoIs0_Sy, in_RoIs0_x, in_RoIs0_y, in_RoIs0_magnitude,
                                    in_n_RoIs0, in_RoIs1_id, in_RoIs1_xmin, in_RoIs1_xmax, in_RoIs1_ymin, in_RoIs1_ymax,
                                    in_RoIs1_S, in_RoIs1_Sx, in_RoIs1_Sy, in_RoIs1_x, in_RoIs1_y, in_RoIs1_magnitude,
                                    in_n_RoIs1, in_frame, status};
    }
}

class Logger_RoIs : public aff3ct::module::Module {
protected:
    const std::string RoIs_path;
    const size_t fra_start;
    const size_t fra_skip;
    const tracking_data_t* tracking_data;
public:
    Logger_RoIs(const std::string RoIs_path, const size_t fra_start, const size_t frame_skip,
                const size_t max_RoIs_size, const tracking_data_t* tracking_data);
    virtual ~Logger_RoIs();
    inline aff3ct::runtime::Task& operator[](const lgr_roi::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_roi::sck::write s);
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "fmdt/aff3ct_wrapper/Logger/Logger_RoIs.hxx"
#endif
