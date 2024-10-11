#include "fmdt/tracking/tracking_io.h"
#include "fmdt/tools.h"

#include "fmdt/spu/Logger/Logger_tracks.hpp"

Logger_tracks::Logger_tracks(const std::string tracks_path, const size_t fra_start,
                             const tracking_data_t* tracking_data)
: spu::module::Stateful(), tracks_path(tracks_path), fra_start(fra_start), tracking_data(tracking_data) {
    assert(tracking_data != NULL);

    const std::string name = "Logger_tracks";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!tracks_path.empty())
        tools_create_folder(tracks_path.c_str());

    this->create_codelet(p, [ps_in_frame]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_trk = static_cast<Logger_tracks&>(m);

        const uint32_t in_frame = *t[ps_in_frame].get_dataptr<const uint32_t>();

        if (in_frame > (uint32_t)lgr_trk.fra_start && !lgr_trk.tracks_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_trk.tracks_path.c_str(), in_frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            tracking_tracks_write_full(file, lgr_trk.tracking_data->tracks);
            fclose(file);
        }

        return spu::runtime::status_t::SUCCESS;
    });
}

Logger_tracks::~Logger_tracks() {}
