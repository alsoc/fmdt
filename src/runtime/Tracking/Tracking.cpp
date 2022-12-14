#include "fmdt/tracking.h"
#include "vec.h"

#include "fmdt/Tracking/Tracking.hpp"

Tracking::Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
                   const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
                   const bool out_bb, const bool magnitude, const size_t max_ROI_size)
: Module(), r_extrapol(r_extrapol), angle_max(angle_max), diff_dev(diff_dev), track_all(track_all),
  magnitude(magnitude), fra_star_min(fra_star_min), fra_meteor_min(fra_meteor_min), fra_meteor_max(fra_meteor_max),
  max_ROI_size(max_ROI_size), tracking_data(nullptr), BB_array(nullptr) {
    const std::string name = "Tracking";
    this->set_name(name);
    this->set_short_name(name);

    this->tracking_data = tracking_alloc_data(std::max(fra_star_min, fra_meteor_min), max_ROI_size);

    if (out_bb)
        this->BB_array = (vec_BB_t*)vector_create();

    auto &p = this->create_task("perform");

    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    auto ps_in_ROI_id = this->template create_socket_in<uint16_t>(p, "in_ROI_id", max_ROI_size);
    auto ps_in_ROI_xmin = this->template create_socket_in<uint16_t>(p, "in_ROI_xmin", max_ROI_size);
    auto ps_in_ROI_xmax = this->template create_socket_in<uint16_t>(p, "in_ROI_xmax", max_ROI_size);
    auto ps_in_ROI_ymin = this->template create_socket_in<uint16_t>(p, "in_ROI_ymin", max_ROI_size);
    auto ps_in_ROI_ymax = this->template create_socket_in<uint16_t>(p, "in_ROI_ymax", max_ROI_size);
    auto ps_in_ROI_x = this->template create_socket_in<float>(p, "in_ROI_x", max_ROI_size);
    auto ps_in_ROI_y = this->template create_socket_in<float>(p, "in_ROI_y", max_ROI_size);
    auto ps_in_ROI_error = this->template create_socket_in<float>(p, "in_ROI_error", max_ROI_size);
    auto ps_in_ROI_prev_id = this->template create_socket_in<int32_t>(p, "in_ROI_prev_id", max_ROI_size);
    auto ps_in_ROI_magnitude = this->template create_socket_in<uint32_t>(p, "in_ROI_magnitude", max_ROI_size);
    auto ps_in_n_ROI = this->template create_socket_in<uint32_t>(p, "in_n_ROI", 1);

    auto ps_in_theta = this->template create_socket_in<double>(p, "in_theta", 1);
    auto ps_in_tx = this->template create_socket_in<double>(p, "in_tx", 1);
    auto ps_in_ty = this->template create_socket_in<double>(p, "in_ty", 1);
    auto ps_in_mean_error = this->template create_socket_in<double>(p, "in_mean_error", 1);
    auto ps_in_std_deviation = this->template create_socket_in<double>(p, "in_std_deviation", 1);

    this->create_codelet(p, [ps_in_frame, ps_in_ROI_id, ps_in_ROI_xmin, ps_in_ROI_xmax, ps_in_ROI_ymin,
                             ps_in_ROI_ymax, ps_in_ROI_x, ps_in_ROI_y, ps_in_ROI_error, ps_in_ROI_prev_id,
                             ps_in_ROI_magnitude, ps_in_n_ROI, ps_in_theta, ps_in_tx, ps_in_ty, ps_in_mean_error,
                             ps_in_std_deviation]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &trk = static_cast<Tracking&>(m);

        const uint32_t n_ROI = *static_cast<const uint32_t*>(t[ps_in_n_ROI].get_dataptr());
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());

        _tracking_perform(trk.tracking_data,
                          static_cast<const uint16_t*>(t[ps_in_ROI_id].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI_xmin].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI_xmax].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI_ymin].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI_ymax].get_dataptr()),
                          static_cast<const float*>(t[ps_in_ROI_x].get_dataptr()),
                          static_cast<const float*>(t[ps_in_ROI_y].get_dataptr()),
                          static_cast<const float*>(t[ps_in_ROI_error].get_dataptr()),
                          static_cast<const int32_t*>(t[ps_in_ROI_prev_id].get_dataptr()),
                          static_cast<const uint32_t*>(t[ps_in_ROI_magnitude].get_dataptr()),
                          n_ROI,
                          &trk.BB_array,
                          frame,
                          *static_cast<double*>(t[ps_in_theta].get_dataptr()),
                          *static_cast<double*>(t[ps_in_tx].get_dataptr()),
                          *static_cast<double*>(t[ps_in_ty].get_dataptr()),
                          *static_cast<double*>(t[ps_in_mean_error].get_dataptr()),
                          *static_cast<double*>(t[ps_in_std_deviation].get_dataptr()),
                          trk.r_extrapol, trk.angle_max, trk.diff_dev, trk.track_all, trk.fra_star_min,
                          trk.fra_meteor_min, trk.fra_meteor_max, trk.magnitude);

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
