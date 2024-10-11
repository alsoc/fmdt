/*!
 * \file
 * \brief C++ wrapper to log tracks.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

#include "fmdt/tracking/tracking_struct.h"

namespace lgr_trk {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_frame, status };
    }
}

class Logger_tracks : public spu::module::Stateful {
protected:
    const std::string tracks_path;
    const size_t fra_start;
    const tracking_data_t* tracking_data;
public:
    Logger_tracks(const std::string tracks_path, const size_t fra_start, const tracking_data_t* tracking_data);
    virtual ~Logger_tracks();
    inline spu::runtime::Task& operator[](const lgr_trk::tsk t);
    inline spu::runtime::Socket& operator[](const lgr_trk::sck::write s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Logger/Logger_tracks.hxx"
