#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

namespace lgr_trk {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_track_id, in_track_begin, in_track_end, in_track_obj_type, in_n_tracks, in_frame,
                                    status };
    }
}

class Logger_track : public aff3ct::module::Module {
protected:
    const std::string tracks_path;
public:
    Logger_track(const std::string tracks_path, const size_t max_tracks_size);
    virtual ~Logger_track();

    inline aff3ct::module::Task& operator[](const lgr_trk::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const lgr_trk::sck::write s) {
        return aff3ct::module::Module::operator[]((size_t)lgr_trk::tsk::write)[(size_t)s];
    }
};
