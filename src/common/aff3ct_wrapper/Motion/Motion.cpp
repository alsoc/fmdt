#include "fmdt/motion/motion_compute.h"

#include "fmdt/aff3ct_wrapper/Motion/Motion.hpp"

Motion::Motion(const size_t max_RoI_size)
: Module(), max_RoI_size(max_RoI_size) {
    const std::string name = "Motion";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("compute");

    auto ps_in_RoI0_x = this->template create_socket_in<float>(p, "in_RoI0_x", max_RoI_size);
    auto ps_in_RoI0_y = this->template create_socket_in<float>(p, "in_RoI0_y", max_RoI_size);
    auto ps_in_RoI1_x = this->template create_socket_in<float>(p, "in_RoI1_x", max_RoI_size);
    auto ps_in_RoI1_y = this->template create_socket_in<float>(p, "in_RoI1_y", max_RoI_size);
    auto ps_in_RoI1_prev_id = this->template create_socket_in<uint32_t>(p, "in_RoI1_prev_id", max_RoI_size);
    auto ps_in_n_RoI1 = this->template create_socket_in<uint32_t>(p, "in_n_RoI1", 1);

    auto ps_out_RoI1_dx = this->template create_socket_out<float>(p, "out_RoI1_dx", max_RoI_size);
    auto ps_out_RoI1_dy = this->template create_socket_out<float>(p, "out_RoI1_dy", max_RoI_size);
    auto ps_out_RoI1_error = this->template create_socket_out<float>(p, "out_RoI1_error", max_RoI_size);
    auto ps_out_RoI1_is_moving = this->template create_socket_out<uint8_t>(p, "out_RoI1_is_moving", max_RoI_size);

    auto ps_out_motion_est1 = this->template create_socket_out<uint8_t>(p, "out_motion_est1", sizeof(motion_t));
    auto ps_out_motion_est2 = this->template create_socket_out<uint8_t>(p, "out_motion_est2", sizeof(motion_t));

    this->create_codelet(p, [ps_in_RoI0_x, ps_in_RoI0_y, ps_in_RoI1_x, ps_in_RoI1_y, ps_out_RoI1_dx, ps_out_RoI1_dy,
                             ps_out_RoI1_error, ps_in_RoI1_prev_id, ps_out_RoI1_is_moving, ps_in_n_RoI1,
                             ps_out_motion_est1, ps_out_motion_est2]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        const uint32_t n_RoI1 = *static_cast<const uint32_t*>(t[ps_in_n_RoI1].get_dataptr());

        _motion_compute(static_cast<const float*>(t[ps_in_RoI0_x].get_dataptr()),
                        static_cast<const float*>(t[ps_in_RoI0_y].get_dataptr()),
                        static_cast<const float*>(t[ps_in_RoI1_x].get_dataptr()),
                        static_cast<const float*>(t[ps_in_RoI1_y].get_dataptr()),
                        static_cast<float*>(t[ps_out_RoI1_dx].get_dataptr()),
                        static_cast<float*>(t[ps_out_RoI1_dy].get_dataptr()),
                        static_cast<float*>(t[ps_out_RoI1_error].get_dataptr()),
                        static_cast<const uint32_t*>(t[ps_in_RoI1_prev_id].get_dataptr()),
                        static_cast<uint8_t*>(t[ps_out_RoI1_is_moving].get_dataptr()),
                        n_RoI1,
                        static_cast<motion_t*>(t[ps_out_motion_est1].get_dataptr()),
                        static_cast<motion_t*>(t[ps_out_motion_est2].get_dataptr()));

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Motion::~Motion() {}
