#include "fmdt/features.h"
#include "fmdt/tools.h"

#include "fmdt/Logger/Logger_ROI_error.hpp"

Logger_ROI_error::Logger_ROI_error(const std::string ROI_error_path, const size_t max_ROI_size)
: Module(), ROI_error_path(ROI_error_path) {
    const std::string name = "Logger_ROI_error";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");
    auto ps_in_ROI_id = this->template create_socket_in<uint16_t>(p, "in_ROI_id", max_ROI_size);
    auto ps_in_ROI_dx = this->template create_socket_in<float>(p, "in_ROI_dx", max_ROI_size);
    auto ps_in_ROI_dy = this->template create_socket_in<float>(p, "in_ROI_dy", max_ROI_size);
    auto ps_in_ROI_error = this->template create_socket_in<float>(p, "in_ROI_error", max_ROI_size);
    auto ps_in_ROI_next_id = this->template create_socket_in<int32_t>(p, "in_ROI_next_id", max_ROI_size);
    auto ps_in_ROI_is_moving = this->template create_socket_in<uint8_t>(p, "in_ROI_is_moving", max_ROI_size);

    auto ps_in_n_ROI = this->template create_socket_in<uint32_t>(p, "in_n_ROI", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!ROI_error_path.empty())
        tools_create_folder(ROI_error_path.c_str());

    this->create_codelet(p, [ps_in_ROI_id, ps_in_ROI_dx, ps_in_ROI_dy, ps_in_ROI_error, ps_in_ROI_next_id,
                             ps_in_ROI_is_moving, ps_in_n_ROI, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_err = static_cast<Logger_ROI_error&>(m);
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        if (frame && !lgr_err.ROI_error_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u_%05u.txt", lgr_err.ROI_error_path.c_str(), frame -1, frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            _features_ROI_error_write(file, frame -1,
                                      static_cast<const uint16_t*>(t[ps_in_ROI_id].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI_dx].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI_dy].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI_error].get_dataptr()),
                                      static_cast<const int32_t*>(t[ps_in_ROI_next_id].get_dataptr()),
                                      static_cast<const uint8_t*>(t[ps_in_ROI_is_moving].get_dataptr()),
                                      *static_cast<const uint32_t*>(t[ps_in_n_ROI].get_dataptr()));
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_ROI_error::~Logger_ROI_error() {}
