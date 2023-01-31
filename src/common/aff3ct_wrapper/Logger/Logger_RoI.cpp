#include "fmdt/features/features_io.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Logger/Logger_RoI.hpp"

Logger_RoI::Logger_RoI(const std::string RoI_path, const size_t fra_start, const size_t fra_skip,
                       const size_t max_RoI_size, const tracking_data_t* tracking_data)
: Module(), RoI_path(RoI_path), fra_start(fra_start), fra_skip(fra_skip), tracking_data(tracking_data) {
    assert(tracking_data != NULL);

    const std::string name = "Logger_RoI";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");
    auto ps_in_RoI0_id = this->template create_socket_in<uint32_t>(p, "in_RoI0_id", max_RoI_size);
    auto ps_in_RoI0_xmin = this->template create_socket_in<uint32_t>(p, "in_RoI0_xmin", max_RoI_size);
    auto ps_in_RoI0_xmax = this->template create_socket_in<uint32_t>(p, "in_RoI0_xmax", max_RoI_size);
    auto ps_in_RoI0_ymin = this->template create_socket_in<uint32_t>(p, "in_RoI0_ymin", max_RoI_size);
    auto ps_in_RoI0_ymax = this->template create_socket_in<uint32_t>(p, "in_RoI0_ymax", max_RoI_size);
    auto ps_in_RoI0_S = this->template create_socket_in<uint32_t>(p, "in_RoI0_S", max_RoI_size);
    auto ps_in_RoI0_Sx = this->template create_socket_in<uint32_t>(p, "in_RoI0_Sx", max_RoI_size);
    auto ps_in_RoI0_Sy = this->template create_socket_in<uint32_t>(p, "in_RoI0_Sy", max_RoI_size);
    auto ps_in_RoI0_x = this->template create_socket_in<float>(p, "in_RoI0_x", max_RoI_size);
    auto ps_in_RoI0_y = this->template create_socket_in<float>(p, "in_RoI0_y", max_RoI_size);
    auto ps_in_RoI0_magnitude = this->template create_socket_in<uint32_t>(p, "in_RoI0_magnitude", max_RoI_size);
    auto ps_in_n_RoI0 = this->template create_socket_in<uint32_t>(p, "in_n_RoI0", 1);

    auto ps_in_RoI1_id = this->template create_socket_in<uint32_t>(p, "in_RoI1_id", max_RoI_size);
    auto ps_in_RoI1_xmin = this->template create_socket_in<uint32_t>(p, "in_RoI1_xmin", max_RoI_size);
    auto ps_in_RoI1_xmax = this->template create_socket_in<uint32_t>(p, "in_RoI1_xmax", max_RoI_size);
    auto ps_in_RoI1_ymin = this->template create_socket_in<uint32_t>(p, "in_RoI1_ymin", max_RoI_size);
    auto ps_in_RoI1_ymax = this->template create_socket_in<uint32_t>(p, "in_RoI1_ymax", max_RoI_size);
    auto ps_in_RoI1_S = this->template create_socket_in<uint32_t>(p, "in_RoI1_S", max_RoI_size);
    auto ps_in_RoI1_Sx = this->template create_socket_in<uint32_t>(p, "in_RoI1_Sx", max_RoI_size);
    auto ps_in_RoI1_Sy = this->template create_socket_in<uint32_t>(p, "in_RoI1_Sy", max_RoI_size);
    auto ps_in_RoI1_x = this->template create_socket_in<float>(p, "in_RoI1_x", max_RoI_size);
    auto ps_in_RoI1_y = this->template create_socket_in<float>(p, "in_RoI1_y", max_RoI_size);
    auto ps_in_RoI1_magnitude = this->template create_socket_in<uint32_t>(p, "in_RoI1_magnitude", max_RoI_size);
    auto ps_in_n_RoI1 = this->template create_socket_in<uint32_t>(p, "in_n_RoI1", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!RoI_path.empty())
        tools_create_folder(RoI_path.c_str());

    this->create_codelet(p, [ps_in_RoI0_id, ps_in_RoI0_xmin, ps_in_RoI0_xmax, ps_in_RoI0_ymin, ps_in_RoI0_ymax,
                             ps_in_RoI0_S, ps_in_RoI0_Sx, ps_in_RoI0_Sy, ps_in_RoI0_x, ps_in_RoI0_y,
                             ps_in_RoI0_magnitude, ps_in_n_RoI0, ps_in_RoI1_id, ps_in_RoI1_xmin, ps_in_RoI1_xmax,
                             ps_in_RoI1_ymin, ps_in_RoI1_ymax, ps_in_RoI1_S, ps_in_RoI1_Sx, ps_in_RoI1_Sy, ps_in_RoI1_x,
                             ps_in_RoI1_y, ps_in_RoI1_magnitude, ps_in_n_RoI1, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_roi = static_cast<Logger_RoI&>(m);

        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        if (!lgr_roi.RoI_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_roi.RoI_path.c_str(), frame);
            FILE* file = fopen(file_path, "w");

            int prev_frame = frame > lgr_roi.fra_start ? (int)frame - (lgr_roi.fra_skip + 1) : -1;

            _features_RoI0_RoI1_write(file, prev_frame, frame,
                                      static_cast<const uint32_t*>(t[ps_in_RoI0_id].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI0_xmin].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI0_xmax].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI0_ymin].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI0_ymax].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI0_S].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI0_Sx].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI0_Sy].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_RoI0_x].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_RoI0_y].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI0_magnitude].get_dataptr()),
                                      *static_cast<const uint32_t*>(t[ps_in_n_RoI0].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI1_id].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI1_xmin].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI1_xmax].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI1_ymin].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI1_ymax].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI1_S].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI1_Sx].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI1_Sy].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_RoI1_x].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_RoI1_y].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoI1_magnitude].get_dataptr()),
                                      *static_cast<const uint32_t*>(t[ps_in_n_RoI1].get_dataptr()),
                                      lgr_roi.tracking_data->tracks);
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_RoI::~Logger_RoI() {}
