#include "fmdt/tracking/tracking_compute.h"
#include "vec.h"

#include "fmdt/aff3ct_wrapper/Tracking/Tracking.hpp"

Tracking::Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
                   const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
                   const bool out_bb, const bool magnitude, const uint8_t extrapol_order_max,
                   const float min_extrapol_ratio_S, const size_t max_RoI_size)
: Module(), r_extrapol(r_extrapol), angle_max(angle_max), diff_dev(diff_dev), track_all(track_all),
  magnitude(magnitude), fra_star_min(fra_star_min), fra_meteor_min(fra_meteor_min), fra_meteor_max(fra_meteor_max),
  extrapol_order_max(extrapol_order_max), min_extrapol_ratio_S(min_extrapol_ratio_S), max_RoI_size(max_RoI_size),
  tracking_data(nullptr), BB_array(nullptr) {
    const std::string name = "Tracking";
    this->set_name(name);
    this->set_short_name(name);

    this->tracking_data = tracking_alloc_data(std::max(fra_star_min, fra_meteor_min), max_RoI_size);

    if (out_bb)
        this->BB_array = (vec_BB_t*)vector_create();

    auto &p = this->create_task("perform");

    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    auto ps_in_RoI_id = this->template create_socket_in<uint32_t>(p, "in_RoI_id", max_RoI_size);
    auto ps_in_RoI_xmin = this->template create_socket_in<uint32_t>(p, "in_RoI_xmin", max_RoI_size);
    auto ps_in_RoI_xmax = this->template create_socket_in<uint32_t>(p, "in_RoI_xmax", max_RoI_size);
    auto ps_in_RoI_ymin = this->template create_socket_in<uint32_t>(p, "in_RoI_ymin", max_RoI_size);
    auto ps_in_RoI_ymax = this->template create_socket_in<uint32_t>(p, "in_RoI_ymax", max_RoI_size);
    auto ps_in_RoI_S = this->template create_socket_in<uint32_t>(p, "in_RoI_S", max_RoI_size);
    auto ps_in_RoI_x = this->template create_socket_in<float>(p, "in_RoI_x", max_RoI_size);
    auto ps_in_RoI_y = this->template create_socket_in<float>(p, "in_RoI_y", max_RoI_size);
    auto ps_in_RoI_error = this->template create_socket_in<float>(p, "in_RoI_error", max_RoI_size);
    auto ps_in_RoI_prev_id = this->template create_socket_in<uint32_t>(p, "in_RoI_prev_id", max_RoI_size);
    auto ps_in_RoI_magnitude = this->template create_socket_in<uint32_t>(p, "in_RoI_magnitude", max_RoI_size);
    auto ps_in_n_RoI = this->template create_socket_in<uint32_t>(p, "in_n_RoI", 1);

    auto ps_in_motion_est = this->template create_socket_in<uint8_t>(p, "in_motion_est", sizeof(motion_t));

    this->create_codelet(p, [ps_in_frame, ps_in_RoI_id, ps_in_RoI_xmin, ps_in_RoI_xmax, ps_in_RoI_ymin,
                             ps_in_RoI_ymax, ps_in_RoI_S, ps_in_RoI_x, ps_in_RoI_y, ps_in_RoI_error, ps_in_RoI_prev_id,
                             ps_in_RoI_magnitude, ps_in_n_RoI, ps_in_motion_est]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &trk = static_cast<Tracking&>(m);

        const uint32_t n_RoI = *static_cast<const uint32_t*>(t[ps_in_n_RoI].get_dataptr());
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());

        _tracking_perform(trk.tracking_data,
                          static_cast<const uint32_t*>(t[ps_in_RoI_id].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoI_xmin].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoI_xmax].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoI_ymin].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoI_ymax].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoI_S].get_dataptr()),
                          static_cast<const float*>(t[ps_in_RoI_x].get_dataptr()),
                          static_cast<const float*>(t[ps_in_RoI_y].get_dataptr()),
                          static_cast<const float*>(t[ps_in_RoI_error].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoI_prev_id].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoI_magnitude].get_dataptr()),
                          n_RoI,
                          &trk.BB_array,
                          frame,
                          static_cast<const motion_t*>(t[ps_in_motion_est].get_dataptr()),
                          trk.r_extrapol, trk.angle_max, trk.diff_dev, trk.track_all, trk.fra_star_min,
                          trk.fra_meteor_min, trk.fra_meteor_max, trk.magnitude, trk.extrapol_order_max,
                          trk.min_extrapol_ratio_S);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Tracking::~Tracking() {
    tracking_free_data(this->tracking_data);
    if (this->BB_array) {
        size_t vs = vector_size(this->BB_array);
        for (size_t i = 0; i < vs; i++)
            vector_free(this->BB_array[i]);
        vector_free(this->BB_array);
    }
}
