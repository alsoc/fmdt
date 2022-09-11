#include "fmdt/tracking.h"
#include "fmdt/tools.h"

#include "fmdt/Logger/Logger_track.hpp"

Logger_track::Logger_track(const std::string tracks_path, const size_t max_tracks_size)
: Module(), tracks_path(tracks_path) {
    const std::string name = "Logger_track";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");
    auto ps_in_track_id = this->template create_socket_in<uint16_t>(p, "in_track_id", max_tracks_size);
    auto ps_in_track_begin = this->template create_socket_in<uint8_t>(p, "in_track_begin", max_tracks_size * sizeof(ROI_light_t));
    auto ps_in_track_end = this->template create_socket_in<uint8_t>(p, "in_track_end", max_tracks_size * sizeof(ROI_light_t));
    auto ps_in_track_obj_type = this->template create_socket_in<uint8_t>(p, "in_track_obj_type", max_tracks_size * sizeof(enum obj_e));
    auto ps_in_n_tracks = this->template create_socket_in<uint32_t>(p, "in_n_tracks", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!tracks_path.empty())
        tools_create_folder(tracks_path.c_str());

    this->create_codelet(p, [ps_in_track_id, ps_in_track_begin, ps_in_track_end, ps_in_track_obj_type,
                             ps_in_n_tracks, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::module::Task &t, const size_t frame_id) -> int {
        auto &lgr_trk = static_cast<Logger_track&>(m);

        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());

        const uint16_t* track_id = static_cast<const uint16_t*>(t[ps_in_track_id].get_dataptr());
        const ROI_light_t* track_begin = static_cast<const ROI_light_t*>(t[ps_in_track_begin].get_dataptr());
        const ROI_light_t* track_end = static_cast<const ROI_light_t*>(t[ps_in_track_end].get_dataptr());
        const enum obj_e* track_obj_type = static_cast<const enum obj_e*>(t[ps_in_track_obj_type].get_dataptr());
        const uint32_t n_tracks = *static_cast<const uint32_t*>(t[ps_in_n_tracks].get_dataptr());

        if (frame && !lgr_trk.tracks_path.empty()) {
            char file_path[256];
            sprintf(file_path, "%s/%05u_%05u.txt", lgr_trk.tracks_path.c_str(), frame -1, frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            _tracking_track_array_write(file, track_id, track_begin, track_end, track_obj_type, n_tracks);
            fclose(file);
        }

        return aff3ct::module::status_t::SUCCESS;
    });
}

Logger_track::~Logger_track() {}
