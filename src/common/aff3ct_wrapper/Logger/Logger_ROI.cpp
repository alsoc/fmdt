#include "fmdt/features/features_io.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Logger/Logger_ROI.hpp"

Logger_ROI::Logger_ROI(const std::string ROI_path, const size_t fra_start, const size_t fra_skip,
                       const size_t max_ROI_size, const tracking_data_t* tracking_data)
: Module(), ROI_path(ROI_path), fra_start(fra_start), fra_skip(fra_skip), tracking_data(tracking_data) {
    assert(tracking_data != NULL);

    const std::string name = "Logger_ROI";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");
    auto ps_in_ROI0_id = this->template create_socket_in<uint32_t>(p, "in_ROI0_id", max_ROI_size);
    auto ps_in_ROI0_xmin = this->template create_socket_in<uint32_t>(p, "in_ROI0_xmin", max_ROI_size);
    auto ps_in_ROI0_xmax = this->template create_socket_in<uint32_t>(p, "in_ROI0_xmax", max_ROI_size);
    auto ps_in_ROI0_ymin = this->template create_socket_in<uint32_t>(p, "in_ROI0_ymin", max_ROI_size);
    auto ps_in_ROI0_ymax = this->template create_socket_in<uint32_t>(p, "in_ROI0_ymax", max_ROI_size);
    auto ps_in_ROI0_S = this->template create_socket_in<uint32_t>(p, "in_ROI0_S", max_ROI_size);
    auto ps_in_ROI0_Sx = this->template create_socket_in<uint32_t>(p, "in_ROI0_Sx", max_ROI_size);
    auto ps_in_ROI0_Sy = this->template create_socket_in<uint32_t>(p, "in_ROI0_Sy", max_ROI_size);
    auto ps_in_ROI0_x = this->template create_socket_in<float>(p, "in_ROI0_x", max_ROI_size);
    auto ps_in_ROI0_y = this->template create_socket_in<float>(p, "in_ROI0_y", max_ROI_size);
    auto ps_in_ROI0_magnitude = this->template create_socket_in<uint32_t>(p, "in_ROI0_magnitude", max_ROI_size);
    auto ps_in_n_ROI0 = this->template create_socket_in<uint32_t>(p, "in_n_ROI0", 1);

    auto ps_in_ROI1_id = this->template create_socket_in<uint32_t>(p, "in_ROI1_id", max_ROI_size);
    auto ps_in_ROI1_xmin = this->template create_socket_in<uint32_t>(p, "in_ROI1_xmin", max_ROI_size);
    auto ps_in_ROI1_xmax = this->template create_socket_in<uint32_t>(p, "in_ROI1_xmax", max_ROI_size);
    auto ps_in_ROI1_ymin = this->template create_socket_in<uint32_t>(p, "in_ROI1_ymin", max_ROI_size);
    auto ps_in_ROI1_ymax = this->template create_socket_in<uint32_t>(p, "in_ROI1_ymax", max_ROI_size);
    auto ps_in_ROI1_S = this->template create_socket_in<uint32_t>(p, "in_ROI1_S", max_ROI_size);
    auto ps_in_ROI1_Sx = this->template create_socket_in<uint32_t>(p, "in_ROI1_Sx", max_ROI_size);
    auto ps_in_ROI1_Sy = this->template create_socket_in<uint32_t>(p, "in_ROI1_Sy", max_ROI_size);
    auto ps_in_ROI1_x = this->template create_socket_in<float>(p, "in_ROI1_x", max_ROI_size);
    auto ps_in_ROI1_y = this->template create_socket_in<float>(p, "in_ROI1_y", max_ROI_size);
    auto ps_in_ROI1_magnitude = this->template create_socket_in<uint32_t>(p, "in_ROI1_magnitude", max_ROI_size);
    auto ps_in_n_ROI1 = this->template create_socket_in<uint32_t>(p, "in_n_ROI1", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!ROI_path.empty())
        tools_create_folder(ROI_path.c_str());

    this->create_codelet(p, [ps_in_ROI0_id, ps_in_ROI0_xmin, ps_in_ROI0_xmax, ps_in_ROI0_ymin, ps_in_ROI0_ymax,
                             ps_in_ROI0_S, ps_in_ROI0_Sx, ps_in_ROI0_Sy, ps_in_ROI0_x, ps_in_ROI0_y,
                             ps_in_ROI0_magnitude, ps_in_n_ROI0, ps_in_ROI1_id, ps_in_ROI1_xmin, ps_in_ROI1_xmax,
                             ps_in_ROI1_ymin, ps_in_ROI1_ymax, ps_in_ROI1_S, ps_in_ROI1_Sx, ps_in_ROI1_Sy, ps_in_ROI1_x,
                             ps_in_ROI1_y, ps_in_ROI1_magnitude, ps_in_n_ROI1, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_roi = static_cast<Logger_ROI&>(m);

        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        if (!lgr_roi.ROI_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_roi.ROI_path.c_str(), frame);
            FILE* file = fopen(file_path, "w");

            int prev_frame = frame > lgr_roi.fra_start ? (int)frame - (lgr_roi.fra_skip + 1) : -1;

            _features_ROI0_ROI1_write(file, prev_frame, frame,
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_id].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_xmin].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_xmax].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_ymin].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_ymax].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_S].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_Sx].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_Sy].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI0_x].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI0_y].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_magnitude].get_dataptr()),
                                      *static_cast<const uint32_t*>(t[ps_in_n_ROI0].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI1_id].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI1_xmin].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI1_xmax].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI1_ymin].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI1_ymax].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI1_S].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI1_Sx].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI1_Sy].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI1_x].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI1_y].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI1_magnitude].get_dataptr()),
                                      *static_cast<const uint32_t*>(t[ps_in_n_ROI1].get_dataptr()),
                                      lgr_roi.tracking_data->tracks);
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_ROI::~Logger_ROI() {}
