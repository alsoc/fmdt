#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

#include "fmdt/features.h"

namespace ftr_mtn {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_ROI0_next_id, in_ROI0_x, in_ROI0_y, in_n_ROI0, in_ROI1_x, in_ROI1_y,
                                      out_ROI0_dx, out_ROI0_dy, out_ROI0_error, out_ROI0_is_moving, out_first_theta,
                                      out_first_tx, out_first_ty, out_first_mean_error, out_first_std_deviation,
                                      out_theta, out_tx, out_ty, out_mean_error, out_std_deviation, status };
    }
}

class Features_motion : public aff3ct::module::Module {
protected:
    const size_t max_ROI_size;
public:
    Features_motion(const size_t max_ROI_size) : Module(), max_ROI_size(max_ROI_size) {
        const std::string name = "Features_motion";
        this->set_name(name);
        this->set_short_name(name);

        auto &p = this->create_task("compute");

        auto ps_in_ROI0_next_id = this->template create_socket_in<int32_t>(p, "in_ROI0_next_id", max_ROI_size);
        auto ps_in_ROI0_x = this->template create_socket_in<float>(p, "in_ROI0_x", max_ROI_size);
        auto ps_in_ROI0_y = this->template create_socket_in<float>(p, "in_ROI0_y", max_ROI_size);
        auto ps_in_n_ROI0 = this->template create_socket_in<uint32_t>(p, "in_n_ROI0", 1);
        auto ps_in_ROI1_x = this->template create_socket_in<float>(p, "in_ROI1_x", max_ROI_size);
        auto ps_in_ROI1_y = this->template create_socket_in<float>(p, "in_ROI1_y", max_ROI_size);

        auto ps_out_ROI0_dx = this->template create_socket_out<float>(p, "out_ROI0_dx", max_ROI_size);
        auto ps_out_ROI0_dy = this->template create_socket_out<float>(p, "out_ROI0_dy", max_ROI_size);
        auto ps_out_ROI0_error = this->template create_socket_out<float>(p, "out_ROI0_error", max_ROI_size);
        auto ps_out_ROI0_is_moving = this->template create_socket_out<uint8_t>(p, "out_ROI0_is_moving", max_ROI_size);

        auto ps_out_first_theta = this->template create_socket_out<double>(p, "out_first_theta", 1);
        auto ps_out_first_tx = this->template create_socket_out<double>(p, "out_first_tx", 1);
        auto ps_out_first_ty = this->template create_socket_out<double>(p, "out_first_ty", 1);
        auto ps_out_first_mean_error = this->template create_socket_out<double>(p, "out_first_mean_error", 1);
        auto ps_out_first_std_deviation = this->template create_socket_out<double>(p, "out_first_std_deviation", 1);

        auto ps_out_theta = this->template create_socket_out<double>(p, "out_theta", 1);
        auto ps_out_tx = this->template create_socket_out<double>(p, "out_tx", 1);
        auto ps_out_ty = this->template create_socket_out<double>(p, "out_ty", 1);
        auto ps_out_mean_error = this->template create_socket_out<double>(p, "out_mean_error", 1);
        auto ps_out_std_deviation = this->template create_socket_out<double>(p, "out_std_deviation", 1);

        this->create_codelet(p, [ps_in_ROI0_next_id, ps_in_ROI0_x, ps_in_ROI0_y, ps_in_n_ROI0, ps_in_ROI1_x,
                                 ps_in_ROI1_y, ps_out_ROI0_dx, ps_out_ROI0_dy, ps_out_ROI0_error,
                                 ps_out_ROI0_is_moving, ps_out_first_theta, ps_out_first_tx, ps_out_first_ty,
                                 ps_out_first_mean_error, ps_out_first_std_deviation, ps_out_theta, ps_out_tx,
                                 ps_out_ty, ps_out_mean_error, ps_out_std_deviation]
                             (aff3ct::module::Module &m, aff3ct::module::Task &t, const size_t frame_id) -> int {
            const uint32_t n_ROI0 = *static_cast<const uint32_t*>(t[ps_in_n_ROI0].get_dataptr());

            std::fill_n(static_cast<uint8_t*>(t[ps_out_ROI0_is_moving].get_dataptr()), n_ROI0, 0);

            _features_compute_motion(static_cast<const int32_t*>(t[ps_in_ROI0_next_id].get_dataptr()),
                                     static_cast<const float*>(t[ps_in_ROI0_x].get_dataptr()),
                                     static_cast<const float*>(t[ps_in_ROI0_y].get_dataptr()),
                                     static_cast<float*>(t[ps_out_ROI0_dx].get_dataptr()),
                                     static_cast<float*>(t[ps_out_ROI0_dy].get_dataptr()),
                                     static_cast<float*>(t[ps_out_ROI0_error].get_dataptr()),
                                     static_cast<uint8_t*>(t[ps_out_ROI0_is_moving].get_dataptr()),
                                     n_ROI0,
                                     static_cast<const float*>(t[ps_in_ROI1_x].get_dataptr()),
                                     static_cast<const float*>(t[ps_in_ROI1_y].get_dataptr()),
                                     static_cast<double*>(t[ps_out_first_theta].get_dataptr()),
                                     static_cast<double*>(t[ps_out_first_tx].get_dataptr()),
                                     static_cast<double*>(t[ps_out_first_ty].get_dataptr()),
                                     static_cast<double*>(t[ps_out_first_mean_error].get_dataptr()),
                                     static_cast<double*>(t[ps_out_first_std_deviation].get_dataptr()),
                                     static_cast<double*>(t[ps_out_theta].get_dataptr()),
                                     static_cast<double*>(t[ps_out_tx].get_dataptr()),
                                     static_cast<double*>(t[ps_out_ty].get_dataptr()),
                                     static_cast<double*>(t[ps_out_mean_error].get_dataptr()),
                                     static_cast<double*>(t[ps_out_std_deviation].get_dataptr()));

            return aff3ct::module::status_t::SUCCESS;
        });
    }

    virtual ~Features_motion() { }

    inline aff3ct::module::Task& operator[](const ftr_mtn::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const ftr_mtn::sck::compute s) {
        return aff3ct::module::Module::operator[]((size_t)ftr_mtn::tsk::compute)[(size_t)s];
    }
};
