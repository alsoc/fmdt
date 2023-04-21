#include "fmdt/motion/motion_compute.h"

#include "fmdt/aff3ct_wrapper/Motion/Motion.hpp"

Motion::Motion(const size_t max_RoIs_size)
: Module(), max_RoIs_size(max_RoIs_size) {
    const std::string name = "Motion";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("compute");

    auto ps_in_RoIs0_x = this->template create_socket_in<float>(p, "in_RoIs0_x", max_RoIs_size);
    auto ps_in_RoIs0_y = this->template create_socket_in<float>(p, "in_RoIs0_y", max_RoIs_size);
    auto ps_in_RoIs1_x = this->template create_socket_in<float>(p, "in_RoIs1_x", max_RoIs_size);
    auto ps_in_RoIs1_y = this->template create_socket_in<float>(p, "in_RoIs1_y", max_RoIs_size);
    auto ps_in_RoIs1_prev_id = this->template create_socket_in<uint32_t>(p, "in_RoIs1_prev_id", max_RoIs_size);
    auto ps_in_n_RoIs1 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs1", 1);

    auto ps_out_RoIs1_dx = this->template create_socket_out<float>(p, "out_RoIs1_dx", max_RoIs_size);
    auto ps_out_RoIs1_dy = this->template create_socket_out<float>(p, "out_RoIs1_dy", max_RoIs_size);
    auto ps_out_RoIs1_error = this->template create_socket_out<float>(p, "out_RoIs1_error", max_RoIs_size);
    auto ps_out_RoIs1_is_moving = this->template create_socket_out<uint8_t>(p, "out_RoIs1_is_moving", max_RoIs_size);

    auto ps_out_motion_est1 = this->template create_socket_out<uint8_t>(p, "out_motion_est1", sizeof(motion_t));
    auto ps_out_motion_est2 = this->template create_socket_out<uint8_t>(p, "out_motion_est2", sizeof(motion_t));

    this->create_codelet(p, [ps_in_RoIs0_x, ps_in_RoIs0_y, ps_in_RoIs1_x, ps_in_RoIs1_y, ps_out_RoIs1_dx,
                             ps_out_RoIs1_dy, ps_out_RoIs1_error, ps_in_RoIs1_prev_id, ps_out_RoIs1_is_moving,
                             ps_in_n_RoIs1, ps_out_motion_est1, ps_out_motion_est2]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        const uint32_t n_RoIs1 = *static_cast<const uint32_t*>(t[ps_in_n_RoIs1].get_dataptr());

        _motion_compute(static_cast<const float*>(t[ps_in_RoIs0_x].get_dataptr()),
                        static_cast<const float*>(t[ps_in_RoIs0_y].get_dataptr()),
                        static_cast<const float*>(t[ps_in_RoIs1_x].get_dataptr()),
                        static_cast<const float*>(t[ps_in_RoIs1_y].get_dataptr()),
                        static_cast<float*>(t[ps_out_RoIs1_dx].get_dataptr()),
                        static_cast<float*>(t[ps_out_RoIs1_dy].get_dataptr()),
                        static_cast<float*>(t[ps_out_RoIs1_error].get_dataptr()),
                        static_cast<const uint32_t*>(t[ps_in_RoIs1_prev_id].get_dataptr()),
                        static_cast<uint8_t*>(t[ps_out_RoIs1_is_moving].get_dataptr()),
                        n_RoIs1,
                        static_cast<motion_t*>(t[ps_out_motion_est1].get_dataptr()),
                        static_cast<motion_t*>(t[ps_out_motion_est2].get_dataptr()));

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Motion::~Motion() {}

Motion* Motion::clone() const {
    auto m = new Motion(*this);
    m->deep_copy(*this);
    return m;
}

void Motion::deep_copy(const Motion &m)
{
    Module::deep_copy(m);
}
