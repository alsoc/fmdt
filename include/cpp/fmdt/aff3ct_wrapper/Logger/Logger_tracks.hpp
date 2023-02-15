/*!
 * \file
 * \brief C++ wrapper to log tracks.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tracking/tracking_struct.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace lgr_trk {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_frame, status };
    }
}
#endif

class Logger_tracks : public aff3ct::module::Module {
protected:
    const std::string tracks_path;
    const size_t fra_start;
    const tracking_data_t* tracking_data;
public:
    Logger_tracks(const std::string tracks_path, const size_t fra_start, const tracking_data_t* tracking_data);
    virtual ~Logger_tracks();
    inline aff3ct::runtime::Task& operator[](const lgr_trk::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_trk::sck::write s);
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "fmdt/aff3ct_wrapper/Logger/Logger_tracks.hxx"
#endif
