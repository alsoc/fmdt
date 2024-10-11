/*!
 * \file
 * \brief C++ wrapper to log RoIs features.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

#include "fmdt/tracking/tracking_struct.h"

namespace lgr_roi {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_RoIs0_basic, in_RoIs0_magn, in_RoIs0_elli, in_n_RoIs0, in_RoIs1_basic,
                                    in_RoIs1_magn, in_RoIs1_elli, in_n_RoIs1, in_frame, status };
    }
}

class Logger_RoIs : public spu::module::Stateful {
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
    inline spu::runtime::Task& operator[](const lgr_roi::tsk t);
    inline spu::runtime::Socket& operator[](const lgr_roi::sck::write s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Logger/Logger_RoIs.hxx"
