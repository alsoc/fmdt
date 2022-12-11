#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace lgr_trk {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_frame, status };
    }
}

class Logger_track : public aff3ct::module::Module {
protected:
    const std::string tracks_path;
    const tracking_data_t* tracking_data;
public:
    Logger_track(const std::string tracks_path, const tracking_data_t* tracking_data);
    virtual ~Logger_track();
    inline aff3ct::runtime::Task& operator[](const lgr_trk::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_trk::sck::write s);
};

#include "fmdt/Logger/Logger_track.hxx"
