#include "fmdt/tracking.h"

#include "fmdt/Tracking/Tracking.hpp"

Tracking::Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
                   const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
                   const bool out_bb, const size_t max_ROI_size, const size_t max_tracks_size,
                   const size_t max_bb_list_size)
: Module(), r_extrapol(r_extrapol), angle_max(angle_max), diff_dev(diff_dev), track_all(track_all),
  fra_star_min(fra_star_min), fra_meteor_min(fra_meteor_min), fra_meteor_max(fra_meteor_max), 
  max_ROI_size(max_ROI_size), max_tracks_size(max_tracks_size), max_bb_list_size(max_bb_list_size),
  ROI0_prev_id(nullptr), tracking_data(nullptr), track_array(nullptr), BB_array(nullptr) {
    const std::string name = "Tracking";
    this->set_name(name);
    this->set_short_name(name);

    this->ROI0_prev_id = (int32_t*)malloc(max_ROI_size * sizeof(uint32_t));
    this->tracking_data = tracking_alloc_data(std::max(fra_star_min, fra_meteor_min), max_ROI_size);
    this->track_array = tracking_alloc_track_array(max_tracks_size);

    if (out_bb)
      this->BB_array = (BB_t**)malloc(max_bb_list_size * sizeof(BB_t*));
    
    memset(this->ROI0_prev_id, 0, max_ROI_size * sizeof(int32_t));
    tracking_init_data(this->tracking_data);
    tracking_init_track_array(this->track_array);
    if (this->BB_array)
      tracking_init_BB_array(this->BB_array);

    auto &p = this->create_task("perform");

    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    auto ps_in_ROI0_id = this->template create_socket_in<uint16_t>(p, "in_ROI0_id", max_ROI_size);
    auto ps_in_ROI0_xmin = this->template create_socket_in<uint16_t>(p, "in_ROI0_xmin", max_ROI_size);
    auto ps_in_ROI0_xmax = this->template create_socket_in<uint16_t>(p, "in_ROI0_xmax", max_ROI_size);
    auto ps_in_ROI0_ymin = this->template create_socket_in<uint16_t>(p, "in_ROI0_ymin", max_ROI_size);
    auto ps_in_ROI0_ymax = this->template create_socket_in<uint16_t>(p, "in_ROI0_ymax", max_ROI_size);
    auto ps_in_ROI0_x = this->template create_socket_in<float>(p, "in_ROI0_x", max_ROI_size);
    auto ps_in_ROI0_y = this->template create_socket_in<float>(p, "in_ROI0_y", max_ROI_size);
    auto ps_in_ROI0_error = this->template create_socket_in<float>(p, "in_ROI0_error", max_ROI_size);
    auto ps_in_ROI0_next_id = this->template create_socket_in<int32_t>(p, "in_ROI0_next_id", max_ROI_size);
    auto ps_in_n_ROI0 = this->template create_socket_in<uint32_t>(p, "in_n_ROI0", 1);

    auto ps_in_ROI1_id = this->template create_socket_in<uint16_t>(p, "in_ROI1_id", max_ROI_size);
    auto ps_in_ROI1_xmin = this->template create_socket_in<uint16_t>(p, "in_ROI1_xmin", max_ROI_size);
    auto ps_in_ROI1_xmax = this->template create_socket_in<uint16_t>(p, "in_ROI1_xmax", max_ROI_size);
    auto ps_in_ROI1_ymin = this->template create_socket_in<uint16_t>(p, "in_ROI1_ymin", max_ROI_size);
    auto ps_in_ROI1_ymax = this->template create_socket_in<uint16_t>(p, "in_ROI1_ymax", max_ROI_size);
    auto ps_in_ROI1_x = this->template create_socket_in<float>(p, "in_ROI1_x", max_ROI_size);
    auto ps_in_ROI1_y = this->template create_socket_in<float>(p, "in_ROI1_y", max_ROI_size);
    auto ps_in_ROI1_prev_id = this->template create_socket_in<int32_t>(p, "in_ROI1_prev_id", max_ROI_size);
    auto ps_in_n_ROI1 = this->template create_socket_in<uint32_t>(p, "in_n_ROI1", 1);

    auto ps_in_theta = this->template create_socket_in<double>(p, "in_theta", 1);
    auto ps_in_tx = this->template create_socket_in<double>(p, "in_tx", 1);
    auto ps_in_ty = this->template create_socket_in<double>(p, "in_ty", 1);
    auto ps_in_mean_error = this->template create_socket_in<double>(p, "in_mean_error", 1);
    auto ps_in_std_deviation = this->template create_socket_in<double>(p, "in_std_deviation", 1);

    auto ps_out_track_id = this->template create_socket_out<uint16_t>(p, "out_track_id", max_tracks_size);
    auto ps_out_track_begin = this->template create_socket_out<uint8_t>(p, "out_track_begin", max_tracks_size * sizeof(ROI_light_t));
    auto ps_out_track_end = this->template create_socket_out<uint8_t>(p, "out_track_end", max_tracks_size * sizeof(ROI_light_t));
    auto ps_out_track_extrapol_x = this->template create_socket_out<float>(p, "out_track_extrapol_x", max_tracks_size);
    auto ps_out_track_extrapol_y = this->template create_socket_out<float>(p, "out_track_extrapol_y", max_tracks_size);
    auto ps_out_track_state = this->template create_socket_out<uint8_t>(p, "out_track_state", max_tracks_size * sizeof(enum state_e));
    auto ps_out_track_obj_type = this->template create_socket_out<uint8_t>(p, "out_track_obj_type", max_tracks_size * sizeof(enum obj_e));
    auto ps_out_track_change_state_reason = this->template create_socket_out<uint8_t>(p, "out_track_change_state_reason", max_tracks_size * sizeof(enum change_state_reason_e));
    auto ps_out_n_tracks = this->template create_socket_out<uint32_t>(p, "out_n_tracks", 1);

    this->create_codelet(p, [ps_in_frame, ps_in_ROI0_id, ps_in_ROI0_xmin, ps_in_ROI0_xmax, ps_in_ROI0_ymin,
                             ps_in_ROI0_ymax, ps_in_ROI0_x, ps_in_ROI0_y, ps_in_ROI0_error, ps_in_ROI0_next_id, 
                             ps_in_n_ROI0, ps_in_ROI1_id, ps_in_ROI1_xmin, ps_in_ROI1_xmax, ps_in_ROI1_ymin, 
                             ps_in_ROI1_ymax, ps_in_ROI1_x, ps_in_ROI1_y, ps_in_ROI1_prev_id, ps_in_n_ROI1, 
                             ps_in_theta, ps_in_tx, ps_in_ty, ps_in_mean_error, ps_in_std_deviation, ps_out_track_id, 
                             ps_out_track_begin, ps_out_track_end, ps_out_track_extrapol_x, ps_out_track_extrapol_y, 
                             ps_out_track_state, ps_out_track_obj_type, ps_out_track_change_state_reason, 
                             ps_out_n_tracks]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &trk = static_cast<Tracking&>(m);

        const uint32_t n_ROI0 = *static_cast<const uint32_t*>(t[ps_in_n_ROI0].get_dataptr());
        const uint32_t n_ROI1 = *static_cast<const uint32_t*>(t[ps_in_n_ROI1].get_dataptr());
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        const int32_t* ROI1_prev_id = static_cast<const int32_t*>(t[ps_in_ROI1_prev_id].get_dataptr());

        _tracking_perform(trk.tracking_data,
                          static_cast<const uint16_t*>(t[ps_in_ROI0_id].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI0_xmin].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI0_xmax].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI0_ymin].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI0_ymax].get_dataptr()),
                          static_cast<const float*>(t[ps_in_ROI0_x].get_dataptr()),
                          static_cast<const float*>(t[ps_in_ROI0_y].get_dataptr()),
                          static_cast<const float*>(t[ps_in_ROI0_error].get_dataptr()),
                          trk.ROI0_prev_id,
                          static_cast<const int32_t*>(t[ps_in_ROI0_next_id].get_dataptr()),
                          n_ROI0,
                          static_cast<const uint16_t*>(t[ps_in_ROI1_id].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI1_xmin].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI1_xmax].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI1_ymin].get_dataptr()),
                          static_cast<const uint16_t*>(t[ps_in_ROI1_ymax].get_dataptr()),
                          static_cast<const float*>(t[ps_in_ROI1_x].get_dataptr()),
                          static_cast<const float*>(t[ps_in_ROI1_y].get_dataptr()),
                          ROI1_prev_id,
                          n_ROI1,
                          trk.track_array->id,
                          trk.track_array->begin,
                          trk.track_array->end,
                          trk.track_array->extrapol_x,
                          trk.track_array->extrapol_y,
                          trk.track_array->state,
                          trk.track_array->obj_type,
                          trk.track_array->change_state_reason,
                          &trk.track_array->_offset,
                          &trk.track_array->_size,
                          trk.BB_array,
                          frame,
                          *static_cast<double*>(t[ps_in_theta].get_dataptr()),
                          *static_cast<double*>(t[ps_in_tx].get_dataptr()),
                          *static_cast<double*>(t[ps_in_ty].get_dataptr()),
                          *static_cast<double*>(t[ps_in_mean_error].get_dataptr()),
                          *static_cast<double*>(t[ps_in_std_deviation].get_dataptr()),
                          trk.r_extrapol, trk.angle_max, trk.diff_dev, trk.track_all, trk.fra_star_min,
                          trk.fra_meteor_min, trk.fra_meteor_max);

        memcpy(trk.ROI0_prev_id, ROI1_prev_id, trk.max_ROI_size * sizeof(int32_t));

        uint16_t* out_track_id = static_cast<uint16_t*>(t[ps_out_track_id].get_dataptr());
        ROI_light_t* out_track_begin = static_cast<ROI_light_t*>(t[ps_out_track_begin].get_dataptr());
        ROI_light_t* out_track_end = static_cast<ROI_light_t*>(t[ps_out_track_end].get_dataptr());
        float* out_track_extrapol_x = static_cast<float*>(t[ps_out_track_extrapol_x].get_dataptr());
        float* out_track_extrapol_y = static_cast<float*>(t[ps_out_track_extrapol_y].get_dataptr());
        enum state_e* out_track_state = static_cast<enum state_e*>(t[ps_out_track_state].get_dataptr());
        enum obj_e* out_track_obj_type = static_cast<enum obj_e*>(t[ps_out_track_obj_type].get_dataptr());
        enum change_state_reason_e* out_track_change_state_reason = static_cast<enum change_state_reason_e*>(t[ps_out_track_change_state_reason].get_dataptr());
        uint32_t* out_n_tracks = static_cast<uint32_t*>(t[ps_out_n_tracks].get_dataptr());

        uint32_t real_n_tracks = 0;
        for (size_t t = trk.track_array->_offset; t < trk.track_array->_size; t++) {
            if (trk.track_array->id[t]) {
                out_track_id[real_n_tracks] = trk.track_array->id[t];
                memcpy(&out_track_begin[real_n_tracks], &trk.track_array->begin[t], sizeof(ROI_light_t));
                memcpy(&out_track_end[real_n_tracks], &trk.track_array->end[t], sizeof(ROI_light_t));
                out_track_extrapol_x[real_n_tracks] = trk.track_array->extrapol_x[t];
                out_track_extrapol_y[real_n_tracks] = trk.track_array->extrapol_y[t];
                out_track_state[real_n_tracks] = trk.track_array->state[t];
                out_track_obj_type[real_n_tracks] = trk.track_array->obj_type[t];
                out_track_change_state_reason[real_n_tracks] = trk.track_array->change_state_reason[t];
                real_n_tracks++;
            }
        }
        *out_n_tracks = real_n_tracks;

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Tracking::~Tracking() {
    tracking_free_data(this->tracking_data);
    tracking_free_track_array(this->track_array);
    if (this->BB_array) {
      tracking_free_BB_array(this->BB_array);
      free(this->BB_array);
    }
}
