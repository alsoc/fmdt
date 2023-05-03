#include "fmdt/features/features_io.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Logger/Logger_RoIs.hpp"

Logger_RoIs::Logger_RoIs(const std::string RoIs_path, const size_t fra_start, const size_t fra_skip,
                         const size_t max_RoIs_size, const tracking_data_t* tracking_data, const bool enable_magnitude,
                         const bool enable_sat_count, const bool enable_ellipse)
: Module(), RoIs_path(RoIs_path), fra_start(fra_start), fra_skip(fra_skip), tracking_data(tracking_data),
  enable_magnitude(enable_magnitude), enable_sat_count(enable_sat_count), enable_ellipse(enable_ellipse)  {
    assert(tracking_data != NULL);

    const std::string name = "Logger_RoIs";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");
    auto ps_in_RoIs0_id = this->template create_socket_in<uint32_t>(p, "in_RoIs0_id", max_RoIs_size);
    auto ps_in_RoIs0_xmin = this->template create_socket_in<uint32_t>(p, "in_RoIs0_xmin", max_RoIs_size);
    auto ps_in_RoIs0_xmax = this->template create_socket_in<uint32_t>(p, "in_RoIs0_xmax", max_RoIs_size);
    auto ps_in_RoIs0_ymin = this->template create_socket_in<uint32_t>(p, "in_RoIs0_ymin", max_RoIs_size);
    auto ps_in_RoIs0_ymax = this->template create_socket_in<uint32_t>(p, "in_RoIs0_ymax", max_RoIs_size);
    auto ps_in_RoIs0_S = this->template create_socket_in<uint32_t>(p, "in_RoIs0_S", max_RoIs_size);
    auto ps_in_RoIs0_Sx = this->template create_socket_in<uint32_t>(p, "in_RoIs0_Sx", max_RoIs_size);
    auto ps_in_RoIs0_Sy = this->template create_socket_in<uint32_t>(p, "in_RoIs0_Sy", max_RoIs_size);
    auto ps_in_RoIs0_Sx2 = this->template create_socket_in<uint64_t>(p, "in_RoIs0_Sx2", max_RoIs_size);
    auto ps_in_RoIs0_Sy2 = this->template create_socket_in<uint64_t>(p, "in_RoIs0_Sy2", max_RoIs_size);
    auto ps_in_RoIs0_Sxy = this->template create_socket_in<uint64_t>(p, "in_RoIs0_Sxy", max_RoIs_size);
    auto ps_in_RoIs0_x = this->template create_socket_in<float>(p, "in_RoIs0_x", max_RoIs_size);
    auto ps_in_RoIs0_y = this->template create_socket_in<float>(p, "in_RoIs0_y", max_RoIs_size);
    auto ps_in_RoIs0_magnitude = this->template create_socket_in<uint32_t>(p, "in_RoIs0_magnitude", max_RoIs_size);
    auto ps_in_RoIs0_sat_count = this->template create_socket_in<uint32_t>(p, "in_RoIs0_sat_count", max_RoIs_size);
    auto ps_in_RoIs0_a = this->template create_socket_in<float>(p, "in_RoIs0_a", max_RoIs_size);
    auto ps_in_RoIs0_b = this->template create_socket_in<float>(p, "in_RoIs0_b", max_RoIs_size);
    auto ps_in_n_RoIs0 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs0", 1);

    auto ps_in_RoIs1_id = this->template create_socket_in<uint32_t>(p, "in_RoIs1_id", max_RoIs_size);
    auto ps_in_RoIs1_xmin = this->template create_socket_in<uint32_t>(p, "in_RoIs1_xmin", max_RoIs_size);
    auto ps_in_RoIs1_xmax = this->template create_socket_in<uint32_t>(p, "in_RoIs1_xmax", max_RoIs_size);
    auto ps_in_RoIs1_ymin = this->template create_socket_in<uint32_t>(p, "in_RoIs1_ymin", max_RoIs_size);
    auto ps_in_RoIs1_ymax = this->template create_socket_in<uint32_t>(p, "in_RoIs1_ymax", max_RoIs_size);
    auto ps_in_RoIs1_S = this->template create_socket_in<uint32_t>(p, "in_RoIs1_S", max_RoIs_size);
    auto ps_in_RoIs1_Sx = this->template create_socket_in<uint32_t>(p, "in_RoIs1_Sx", max_RoIs_size);
    auto ps_in_RoIs1_Sy = this->template create_socket_in<uint32_t>(p, "in_RoIs1_Sy", max_RoIs_size);
    auto ps_in_RoIs1_Sx2 = this->template create_socket_in<uint64_t>(p, "in_RoIs1_Sx2", max_RoIs_size);
    auto ps_in_RoIs1_Sy2 = this->template create_socket_in<uint64_t>(p, "in_RoIs1_Sy2", max_RoIs_size);
    auto ps_in_RoIs1_Sxy = this->template create_socket_in<uint64_t>(p, "in_RoIs1_Sxy", max_RoIs_size);
    auto ps_in_RoIs1_x = this->template create_socket_in<float>(p, "in_RoIs1_x", max_RoIs_size);
    auto ps_in_RoIs1_y = this->template create_socket_in<float>(p, "in_RoIs1_y", max_RoIs_size);
    auto ps_in_RoIs1_magnitude = this->template create_socket_in<uint32_t>(p, "in_RoIs1_magnitude", max_RoIs_size);
    auto ps_in_RoIs1_sat_count = this->template create_socket_in<uint32_t>(p, "in_RoIs1_sat_count", max_RoIs_size);
    auto ps_in_RoIs1_a = this->template create_socket_in<float>(p, "in_RoIs1_a", max_RoIs_size);
    auto ps_in_RoIs1_b = this->template create_socket_in<float>(p, "in_RoIs1_b", max_RoIs_size);
    auto ps_in_n_RoIs1 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs1", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!RoIs_path.empty())
        tools_create_folder(RoIs_path.c_str());

    this->create_codelet(p, [ps_in_RoIs0_id, ps_in_RoIs0_xmin, ps_in_RoIs0_xmax, ps_in_RoIs0_ymin, ps_in_RoIs0_ymax,
                             ps_in_RoIs0_S, ps_in_RoIs0_Sx, ps_in_RoIs0_Sy, ps_in_RoIs0_Sx2, ps_in_RoIs0_Sy2,
                             ps_in_RoIs0_Sxy, ps_in_RoIs0_x, ps_in_RoIs0_y, ps_in_RoIs0_magnitude,
                             ps_in_RoIs0_sat_count, ps_in_RoIs0_a, ps_in_RoIs0_b, ps_in_n_RoIs0, ps_in_RoIs1_id,
                             ps_in_RoIs1_xmin, ps_in_RoIs1_xmax, ps_in_RoIs1_ymin, ps_in_RoIs1_ymax, ps_in_RoIs1_S,
                             ps_in_RoIs1_Sx, ps_in_RoIs1_Sy, ps_in_RoIs1_Sx2, ps_in_RoIs1_Sy2, ps_in_RoIs1_Sxy,
                             ps_in_RoIs1_x, ps_in_RoIs1_y, ps_in_RoIs1_magnitude, ps_in_RoIs1_sat_count, ps_in_RoIs1_a,
                             ps_in_RoIs1_b, ps_in_n_RoIs1, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_roi = static_cast<Logger_RoIs&>(m);

        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        if (!lgr_roi.RoIs_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_roi.RoIs_path.c_str(), frame);
            FILE* file = fopen(file_path, "w");

            int prev_frame = frame > lgr_roi.fra_start ? (int)frame - (lgr_roi.fra_skip + 1) : -1;

            _features_RoIs0_RoIs1_write(file, prev_frame, frame,
                                        static_cast<const uint32_t*>(t[ps_in_RoIs0_id].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs0_xmin].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs0_xmax].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs0_ymin].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs0_ymax].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs0_S].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs0_Sx].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs0_Sy].get_dataptr()),
                                        static_cast<const uint64_t*>(t[ps_in_RoIs0_Sx2].get_dataptr()),
                                        static_cast<const uint64_t*>(t[ps_in_RoIs0_Sy2].get_dataptr()),
                                        static_cast<const uint64_t*>(t[ps_in_RoIs0_Sxy].get_dataptr()),
                                        static_cast<const float*>(t[ps_in_RoIs0_x].get_dataptr()),
                                        static_cast<const float*>(t[ps_in_RoIs0_y].get_dataptr()),
                                        lgr_roi.enable_magnitude ?
                                            static_cast<const uint32_t*>(t[ps_in_RoIs0_magnitude].get_dataptr()) :
                                            nullptr,
                                        lgr_roi.enable_sat_count ?
                                            static_cast<const uint32_t*>(t[ps_in_RoIs0_sat_count].get_dataptr()) :
                                            nullptr,
                                        lgr_roi.enable_ellipse ?
                                            static_cast<const float*>(t[ps_in_RoIs0_a].get_dataptr()) :
                                            nullptr,
                                        lgr_roi.enable_ellipse ?
                                            static_cast<const float*>(t[ps_in_RoIs0_b].get_dataptr()) :
                                            nullptr,
                                        *static_cast<const uint32_t*>(t[ps_in_n_RoIs0].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs1_id].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs1_xmin].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs1_xmax].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs1_ymin].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs1_ymax].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs1_S].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs1_Sx].get_dataptr()),
                                        static_cast<const uint32_t*>(t[ps_in_RoIs1_Sy].get_dataptr()),
                                        static_cast<const uint64_t*>(t[ps_in_RoIs1_Sx2].get_dataptr()),
                                        static_cast<const uint64_t*>(t[ps_in_RoIs1_Sy2].get_dataptr()),
                                        static_cast<const uint64_t*>(t[ps_in_RoIs1_Sxy].get_dataptr()),
                                        static_cast<const float*>(t[ps_in_RoIs1_x].get_dataptr()),
                                        static_cast<const float*>(t[ps_in_RoIs1_y].get_dataptr()),
                                        lgr_roi.enable_magnitude ?
                                            static_cast<const uint32_t*>(t[ps_in_RoIs1_magnitude].get_dataptr()) :
                                            nullptr,
                                        lgr_roi.enable_sat_count ?
                                            static_cast<const uint32_t*>(t[ps_in_RoIs1_sat_count].get_dataptr()) :
                                            nullptr,
                                        lgr_roi.enable_ellipse ?
                                            static_cast<const float*>(t[ps_in_RoIs1_a].get_dataptr()) :
                                            nullptr,
                                        lgr_roi.enable_ellipse ?
                                            static_cast<const float*>(t[ps_in_RoIs1_b].get_dataptr()) :
                                            nullptr,
                                        *static_cast<const uint32_t*>(t[ps_in_n_RoIs1].get_dataptr()),
                                        lgr_roi.tracking_data->tracks);
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_RoIs::~Logger_RoIs() {}
