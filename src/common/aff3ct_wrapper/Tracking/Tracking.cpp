#include "fmdt/tracking/tracking_compute.h"
#include "vec.h"

#include "fmdt/aff3ct_wrapper/Tracking/Tracking.hpp"

Tracking::Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
                   const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
                   const bool out_bb, const bool save_RoIs_id, const uint8_t extrapol_order_max,
                   const float min_extrapol_ratio_S, const size_t max_RoIs_size)
: Module(), r_extrapol(r_extrapol), angle_max(angle_max), diff_dev(diff_dev), track_all(track_all),
  save_RoIs_id(save_RoIs_id), fra_star_min(fra_star_min), fra_meteor_min(fra_meteor_min),
  fra_meteor_max(fra_meteor_max), extrapol_order_max(extrapol_order_max), min_extrapol_ratio_S(min_extrapol_ratio_S),
  max_RoIs_size(max_RoIs_size), tracking_data(nullptr), BBs(nullptr) {
    const std::string name = "Tracking";
    this->set_name(name);
    this->set_short_name(name);

    this->tracking_data = tracking_alloc_data(std::max(fra_star_min, fra_meteor_min), max_RoIs_size);

    if (out_bb)
        this->BBs = (vec_BB_t*)vector_create();

    auto &p = this->create_task("perform");

    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    auto ps_in_RoIs_id = this->template create_socket_in<uint32_t>(p, "in_RoIs_id", max_RoIs_size);
    auto ps_in_RoIs_xmin = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmin", max_RoIs_size);
    auto ps_in_RoIs_xmax = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmax", max_RoIs_size);
    auto ps_in_RoIs_ymin = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymin", max_RoIs_size);
    auto ps_in_RoIs_ymax = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymax", max_RoIs_size);
    auto ps_in_RoIs_S = this->template create_socket_in<uint32_t>(p, "in_RoIs_S", max_RoIs_size);
    auto ps_in_RoIs_x = this->template create_socket_in<float>(p, "in_RoIs_x", max_RoIs_size);
    auto ps_in_RoIs_y = this->template create_socket_in<float>(p, "in_RoIs_y", max_RoIs_size);
    auto ps_in_RoIs_error = this->template create_socket_in<float>(p, "in_RoIs_error", max_RoIs_size);
    auto ps_in_RoIs_prev_id = this->template create_socket_in<uint32_t>(p, "in_RoIs_prev_id", max_RoIs_size);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);

    auto ps_in_motion_est = this->template create_socket_in<uint8_t>(p, "in_motion_est", sizeof(motion_t));

    this->create_codelet(p, [ps_in_frame, ps_in_RoIs_id, ps_in_RoIs_xmin, ps_in_RoIs_xmax, ps_in_RoIs_ymin,
                             ps_in_RoIs_ymax, ps_in_RoIs_S, ps_in_RoIs_x, ps_in_RoIs_y, ps_in_RoIs_error,
                             ps_in_RoIs_prev_id, ps_in_n_RoIs, ps_in_motion_est]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &trk = static_cast<Tracking&>(m);

        const uint32_t n_RoIs = *static_cast<const uint32_t*>(t[ps_in_n_RoIs].get_dataptr());
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());

        _tracking_perform(trk.tracking_data,
                          static_cast<const uint32_t*>(t[ps_in_RoIs_id].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoIs_xmin].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoIs_xmax].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoIs_ymin].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoIs_ymax].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoIs_S].get_dataptr()),
                          static_cast<const float*>(t[ps_in_RoIs_x].get_dataptr()),
                          static_cast<const float*>(t[ps_in_RoIs_y].get_dataptr()),
                          static_cast<const float*>(t[ps_in_RoIs_error].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_RoIs_prev_id].get_dataptr()),
                          n_RoIs,
                          &trk.BBs,
                          frame,
                          static_cast<const motion_t*>(t[ps_in_motion_est].get_dataptr()),
                          trk.r_extrapol, trk.angle_max, trk.diff_dev, trk.track_all, trk.fra_star_min,
                          trk.fra_meteor_min, trk.fra_meteor_max, trk.save_RoIs_id, trk.extrapol_order_max,
                          trk.min_extrapol_ratio_S);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Tracking::~Tracking() {
    tracking_free_data(this->tracking_data);
    if (this->BBs) {
        size_t vs = vector_size(this->BBs);
        for (size_t i = 0; i < vs; i++)
            vector_free(this->BBs[i]);
        vector_free(this->BBs);
    }
}
