#include "fmdt/features.h"
#include "fmdt/tools.h"

#include "fmdt/Logger/Logger_motion.hpp"

Logger_motion::Logger_motion(const std::string motion_path)
: Module(), motion_path(motion_path) {
    const std::string name = "Logger_motion";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");
    auto ps_in_first_theta = this->template create_socket_in<double>(p, "in_first_theta", 1);
    auto ps_in_first_tx = this->template create_socket_in<double>(p, "in_first_tx", 1);
    auto ps_in_first_ty = this->template create_socket_in<double>(p, "in_first_ty", 1);
    auto ps_in_first_mean_error = this->template create_socket_in<double>(p, "in_first_mean_error", 1);
    auto ps_in_first_std_deviation = this->template create_socket_in<double>(p, "in_first_std_deviation", 1);
    auto ps_in_theta = this->template create_socket_in<double>(p, "in_theta", 1);
    auto ps_in_tx = this->template create_socket_in<double>(p, "in_tx", 1);
    auto ps_in_ty = this->template create_socket_in<double>(p, "in_ty", 1);
    auto ps_in_mean_error = this->template create_socket_in<double>(p, "in_mean_error", 1);
    auto ps_in_std_deviation = this->template create_socket_in<double>(p, "in_std_deviation", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!motion_path.empty())
        tools_create_folder(motion_path.c_str());

    this->create_codelet(p, [ps_in_first_theta, ps_in_first_tx, ps_in_first_ty, ps_in_first_mean_error,
                             ps_in_first_std_deviation, ps_in_theta, ps_in_tx, ps_in_ty, ps_in_mean_error,
                             ps_in_std_deviation, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_mtn = static_cast<Logger_motion&>(m);
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        if (frame && !lgr_mtn.motion_path.empty()) {
            char file_path[256];
            snprintf(file_path, 256, "%s/%05u_%05u.txt", lgr_mtn.motion_path.c_str(), frame -1, frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            features_motion_write(file,
                                  *static_cast<const double*>(t[ps_in_first_theta].get_dataptr()),
                                  *static_cast<const double*>(t[ps_in_first_tx].get_dataptr()),
                                  *static_cast<const double*>(t[ps_in_first_ty].get_dataptr()),
                                  *static_cast<const double*>(t[ps_in_first_mean_error].get_dataptr()),
                                  *static_cast<const double*>(t[ps_in_first_std_deviation].get_dataptr()),
                                  *static_cast<const double*>(t[ps_in_theta].get_dataptr()),
                                  *static_cast<const double*>(t[ps_in_tx].get_dataptr()),
                                  *static_cast<const double*>(t[ps_in_ty].get_dataptr()),
                                  *static_cast<const double*>(t[ps_in_mean_error].get_dataptr()),
                                  *static_cast<const double*>(t[ps_in_std_deviation].get_dataptr()));
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_motion::~Logger_motion() {}
