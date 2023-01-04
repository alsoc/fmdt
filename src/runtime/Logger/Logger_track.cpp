#include "fmdt/tracking.h"
#include "fmdt/tools.h"

#include "fmdt/Logger/Logger_track.hpp"

Logger_track::Logger_track(const std::string tracks_path, const size_t fra_start, const tracking_data_t* tracking_data)
: Module(), tracks_path(tracks_path), fra_start(fra_start), tracking_data(tracking_data) {
    assert(tracking_data != NULL);

    const std::string name = "Logger_track";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!tracks_path.empty())
        tools_create_folder(tracks_path.c_str());

    this->create_codelet(p, [ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_trk = static_cast<Logger_track&>(m);

        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());

        if (frame > (uint32_t)lgr_trk.fra_start && !lgr_trk.tracks_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_trk.tracks_path.c_str(), frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            tracking_track_array_write_full(file, lgr_trk.tracking_data->tracks);
            fclose(file);
        }

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_track::~Logger_track() {}
