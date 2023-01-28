#include "fmdt/features/features_io.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Logger/Logger_motion.hpp"

Logger_motion::Logger_motion(const std::string motion_path, const size_t fra_start)
: Module(), motion_path(motion_path), fra_start(fra_start) {
    const std::string name = "Logger_motion";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");
    auto ps_in_motion_est1 = this->template create_socket_in<uint8_t>(p, "in_motion_est1", sizeof(motion_t));
    auto ps_in_motion_est2 = this->template create_socket_in<uint8_t>(p, "in_motion_est2", sizeof(motion_t));
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!motion_path.empty())
        tools_create_folder(motion_path.c_str());

    this->create_codelet(p, [ps_in_motion_est1, ps_in_motion_est2, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_mtn = static_cast<Logger_motion&>(m);
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        if (frame > (uint32_t)lgr_mtn.fra_start && !lgr_mtn.motion_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_mtn.motion_path.c_str(), frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            features_motion_write(file,
                                  static_cast<const motion_t*>(t[ps_in_motion_est1].get_dataptr()),
                                  static_cast<const motion_t*>(t[ps_in_motion_est2].get_dataptr()));
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_motion::~Logger_motion() {}
