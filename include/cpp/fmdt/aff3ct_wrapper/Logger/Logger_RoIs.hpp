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
        enum class write : size_t { in_RoIs0_basic, in_RoIs0_magn, in_RoIs0_elli, in_n_RoIs0, in_RoIs1_basic,
                                    in_RoIs1_magn, in_RoIs1_elli, in_n_RoIs1, in_frame, status };
    }
}

class Logger_RoIs : public aff3ct::module::Module {
protected:
    const std::string RoIs_path;
    const size_t fra_start;
    const size_t fra_skip;
    const tracking_data_t* tracking_data;
    const bool enable_magnitude;
    const bool enable_sat_count;
    const bool enable_ellipse;
public:
    Logger_RoIs(const std::string RoIs_path, const size_t fra_start, const size_t frame_skip,
                const size_t max_RoIs_size, const tracking_data_t* tracking_data, const bool enable_magnitude = false,
                const bool enable_sat_count = false, const bool enable_ellipse = false);
    virtual ~Logger_RoIs();
    inline aff3ct::runtime::Task& operator[](const lgr_roi::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_roi::sck::write s);
    inline aff3ct::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/aff3ct_wrapper/Logger/Logger_RoIs.hxx"
