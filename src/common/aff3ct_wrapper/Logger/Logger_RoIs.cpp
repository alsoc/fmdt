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
    auto ps_in_RoIs0_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs0_basic",
                                                                      max_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_RoIs0_magn = this->template create_socket_in<uint8_t>(p, "in_RoIs0_magn",
                                                                     max_RoIs_size * sizeof(RoI_magn_t));
    auto ps_in_RoIs0_elli = this->template create_socket_in<uint8_t>(p, "in_RoIs0_elli",
                                                                     max_RoIs_size * sizeof(RoI_elli_t));
    auto ps_in_n_RoIs0 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs0", 1);

    auto ps_in_RoIs1_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs1_basic",
                                                                      max_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_RoIs1_magn = this->template create_socket_in<uint8_t>(p, "in_RoIs1_magn",
                                                                     max_RoIs_size * sizeof(RoI_magn_t));
    auto ps_in_RoIs1_elli = this->template create_socket_in<uint8_t>(p, "in_RoIs1_elli",
                                                                     max_RoIs_size * sizeof(RoI_elli_t));
    auto ps_in_n_RoIs1 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs1", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!RoIs_path.empty())
        tools_create_folder(RoIs_path.c_str());

    this->create_codelet(p, [ps_in_RoIs0_basic, ps_in_RoIs0_magn, ps_in_RoIs0_elli, ps_in_n_RoIs0, ps_in_RoIs1_basic,
                             ps_in_RoIs1_magn, ps_in_RoIs1_elli, ps_in_n_RoIs1, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_roi = static_cast<Logger_RoIs&>(m);

        const RoI_basic_t* in_RoIs0_basic  =  t[ps_in_RoIs0_basic].get_dataptr<const RoI_basic_t>();
        const RoI_magn_t*  in_RoIs0_magn   =  t[ps_in_RoIs0_magn ].get_dataptr<const RoI_magn_t >();
        const RoI_elli_t*  in_RoIs0_elli   =  t[ps_in_RoIs0_elli ].get_dataptr<const RoI_elli_t >();
        const uint32_t     in_n_RoIs0      = *t[ps_in_n_RoIs0    ].get_dataptr<const uint32_t   >();
        const RoI_basic_t* in_RoIs1_basic  =  t[ps_in_RoIs1_basic].get_dataptr<const RoI_basic_t>();
        const RoI_magn_t*  in_RoIs1_magn   =  t[ps_in_RoIs1_magn ].get_dataptr<const RoI_magn_t >();
        const RoI_elli_t*  in_RoIs1_elli   =  t[ps_in_RoIs1_elli ].get_dataptr<const RoI_elli_t >();
        const uint32_t     in_n_RoIs1      = *t[ps_in_n_RoIs1    ].get_dataptr<const uint32_t   >();
        const uint32_t     in_frame        = *t[ps_in_frame      ].get_dataptr<const uint32_t   >();

        if (!lgr_roi.RoIs_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_roi.RoIs_path.c_str(), in_frame);
            FILE* file = fopen(file_path, "w");

            int prev_frame = in_frame > lgr_roi.fra_start ? (int)in_frame - (lgr_roi.fra_skip + 1) : -1;

            features_RoIs0_RoIs1_write(file, prev_frame, in_frame,
                                       in_RoIs0_basic,
                                       lgr_roi.enable_magnitude ? in_RoIs0_magn : nullptr,
                                       lgr_roi.enable_ellipse ? in_RoIs0_elli : nullptr,
                                       in_n_RoIs0,
                                       in_RoIs1_basic,
                                       lgr_roi.enable_magnitude ? in_RoIs1_magn : nullptr,
                                       lgr_roi.enable_ellipse ? in_RoIs1_elli : nullptr,
                                       in_n_RoIs1,
                                       lgr_roi.tracking_data->tracks);
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_RoIs::~Logger_RoIs() {}
