#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

#include "fmdt/tracking.h"

namespace trk {
    enum class tsk : size_t { perform, SIZE };
    namespace sck {
        enum class perform : size_t { in_frame, in_ROI0_id, in_ROI0_frame, in_ROI0_xmin, in_ROI0_xmax, in_ROI0_ymin,
                                      in_ROI0_ymax, in_ROI0_x, in_ROI0_y, in_ROI0_error, in_ROI0_time,
                                      in_ROI0_time_motion, in_ROI0_prev_id, in_ROI0_next_id, in_ROI0_is_extrapolated,
                                      in_n_ROI0, in_ROI1_id, in_ROI1_xmin, in_ROI1_xmax, in_ROI1_ymin, in_ROI1_ymax,
                                      in_ROI1_x, in_ROI1_y, in_ROI1_prev_id, in_n_ROI1, in_theta, in_tx, in_ty,
                                      in_mean_error, in_std_deviation, out_ROI1_frame, out_ROI1_time,
                                      out_ROI1_time_motion, out_ROI1_is_extrapolated, status };
    }
}

class Tracking : public aff3ct::module::Module {
protected:
    const size_t r_extrapol;
    const float angle_max;
    const float diff_dev;
    const int track_all;
    const size_t fra_star_min;
    const size_t fra_meteor_min;
    const size_t fra_meteor_max;
    const size_t max_ROI_size;
    const size_t max_tracks_size;
    const size_t max_n_frames;

    tracking_data_t* tracking_data;
    track_t* track_array;
    BB_t** BB_array;

public:
    Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
             const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
             const size_t max_ROI_size, const size_t max_tracks_size, const size_t max_n_frames)
    : Module(), r_extrapol(r_extrapol), angle_max(angle_max), diff_dev(diff_dev), track_all(track_all),
      fra_star_min(fra_star_min), fra_meteor_min(fra_meteor_min), fra_meteor_max(fra_meteor_max),
      max_ROI_size(max_ROI_size), max_tracks_size(max_tracks_size), max_n_frames(max_n_frames), tracking_data(nullptr),
      track_array(nullptr), BB_array(nullptr) {
        const std::string name = "Tracking";
        this->set_name(name);
        this->set_short_name(name);

        this->tracking_data = tracking_alloc_data(std::max(fra_star_min, fra_meteor_min), max_ROI_size);
        this->track_array = tracking_alloc_track_array(max_tracks_size);
        this->BB_array = (BB_t**)malloc(max_n_frames * sizeof(BB_t*));

        tracking_init_data(this->tracking_data);
        tracking_init_track_array(this->track_array);
        tracking_init_BB_array(this->BB_array);

        auto &p = this->create_task("perform");

        auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

        auto ps_in_ROI0_id = this->template create_socket_in<uint16_t>(p, "in_ROI0_id", max_ROI_size);
        auto ps_in_ROI0_frame = this->template create_socket_in<uint32_t>(p, "in_ROI0_frame", max_ROI_size);
        auto ps_in_ROI0_xmin = this->template create_socket_in<uint16_t>(p, "in_ROI0_xmin", max_ROI_size);
        auto ps_in_ROI0_xmax = this->template create_socket_in<uint16_t>(p, "in_ROI0_xmax", max_ROI_size);
        auto ps_in_ROI0_ymin = this->template create_socket_in<uint16_t>(p, "in_ROI0_ymin", max_ROI_size);
        auto ps_in_ROI0_ymax = this->template create_socket_in<uint16_t>(p, "in_ROI0_ymax", max_ROI_size);
        auto ps_in_ROI0_x = this->template create_socket_in<float>(p, "in_ROI0_x", max_ROI_size);
        auto ps_in_ROI0_y = this->template create_socket_in<float>(p, "in_ROI0_y", max_ROI_size);
        auto ps_in_ROI0_error = this->template create_socket_in<float>(p, "in_ROI0_error", max_ROI_size);
        auto ps_in_ROI0_time = this->template create_socket_in<int32_t>(p, "in_ROI0_time", max_ROI_size);
        auto ps_in_ROI0_time_motion = this->template create_socket_in<int32_t>(p, "in_ROI0_time_motion", max_ROI_size);
        auto ps_in_ROI0_prev_id = this->template create_socket_in<int32_t>(p, "in_ROI0_prev_id", max_ROI_size);
        auto ps_in_ROI0_next_id = this->template create_socket_in<int32_t>(p, "in_ROI0_next_id", max_ROI_size);
        auto ps_in_ROI0_is_extrapolated = this->template create_socket_in<uint8_t>(p, "in_ROI0_is_extrapolated", max_ROI_size);
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

        auto ps_out_ROI1_frame = this->template create_socket_out<uint32_t>(p, "out_ROI1_frame", max_ROI_size);
        auto ps_out_ROI1_time = this->template create_socket_out<int32_t>(p, "out_ROI1_time", max_ROI_size);
        auto ps_out_ROI1_time_motion = this->template create_socket_out<int32_t>(p, "out_ROI1_time_motion", max_ROI_size);
        auto ps_out_ROI1_is_extrapolated = this->template create_socket_out<uint8_t>(p, "out_ROI1_is_extrapolated", max_ROI_size);

        this->create_codelet(p, [ps_in_frame, ps_in_ROI0_id, ps_in_ROI0_frame, ps_in_ROI0_xmin, ps_in_ROI0_xmax,
                                 ps_in_ROI0_ymin, ps_in_ROI0_ymax, ps_in_ROI0_x, ps_in_ROI0_y, ps_in_ROI0_error,
                                 ps_in_ROI0_time, ps_in_ROI0_time_motion, ps_in_ROI0_prev_id, ps_in_ROI0_next_id,
                                 ps_in_ROI0_is_extrapolated, ps_in_n_ROI0, ps_in_ROI1_id, ps_in_ROI1_xmin,
                                 ps_in_ROI1_xmax, ps_in_ROI1_ymin, ps_in_ROI1_ymax, ps_in_ROI1_x, ps_in_ROI1_y,
                                 ps_in_ROI1_prev_id, ps_in_n_ROI1, ps_in_theta, ps_in_tx, ps_in_ty, ps_in_mean_error,
                                 ps_in_std_deviation, ps_out_ROI1_frame, ps_out_ROI1_time, ps_out_ROI1_time_motion,
                                 ps_out_ROI1_is_extrapolated]
                             (aff3ct::module::Module &m, aff3ct::module::Task &t, const size_t frame_id) -> int {
            auto &trk = static_cast<Tracking&>(m);

            const uint32_t n_ROI0 = *static_cast<const uint32_t*>(t[ps_in_n_ROI0].get_dataptr());
            const uint32_t n_ROI1 = *static_cast<const uint32_t*>(t[ps_in_n_ROI1].get_dataptr());
            const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());

            std::fill_n(static_cast<uint32_t*>(t[ps_out_ROI1_frame].get_dataptr()), n_ROI1, frame);
            std::fill_n(static_cast<int32_t*>(t[ps_out_ROI1_time].get_dataptr()), n_ROI1, 0);
            std::fill_n(static_cast<int32_t*>(t[ps_out_ROI1_time_motion].get_dataptr()), n_ROI1, 0);
            std::fill_n(static_cast<uint8_t*>(t[ps_out_ROI1_is_extrapolated].get_dataptr()), n_ROI1, 0);

            _tracking_perform(trk.tracking_data,
                              static_cast<const uint16_t*>(t[ps_in_ROI0_id].get_dataptr()),
                              static_cast<const uint32_t*>(t[ps_in_ROI0_frame].get_dataptr()),
                              static_cast<const uint16_t*>(t[ps_in_ROI0_xmin].get_dataptr()),
                              static_cast<const uint16_t*>(t[ps_in_ROI0_xmax].get_dataptr()),
                              static_cast<const uint16_t*>(t[ps_in_ROI0_ymin].get_dataptr()),
                              static_cast<const uint16_t*>(t[ps_in_ROI0_ymax].get_dataptr()),
                              static_cast<const float*>(t[ps_in_ROI0_x].get_dataptr()),
                              static_cast<const float*>(t[ps_in_ROI0_y].get_dataptr()),
                              static_cast<const float*>(t[ps_in_ROI0_error].get_dataptr()),
                              static_cast<const int32_t*>(t[ps_in_ROI0_time].get_dataptr()),
                              static_cast<const int32_t*>(t[ps_in_ROI0_time_motion].get_dataptr()),
                              static_cast<const int32_t*>(t[ps_in_ROI0_prev_id].get_dataptr()),
                              static_cast<const int32_t*>(t[ps_in_ROI0_next_id].get_dataptr()),
                              static_cast<const uint8_t*>(t[ps_in_ROI0_is_extrapolated].get_dataptr()),
                              n_ROI0,
                              static_cast<const uint16_t*>(t[ps_in_ROI1_id].get_dataptr()),
                              static_cast<const uint32_t*>(t[ps_out_ROI1_frame].get_dataptr()),
                              static_cast<const uint16_t*>(t[ps_in_ROI1_xmin].get_dataptr()),
                              static_cast<const uint16_t*>(t[ps_in_ROI1_xmax].get_dataptr()),
                              static_cast<const uint16_t*>(t[ps_in_ROI1_ymin].get_dataptr()),
                              static_cast<const uint16_t*>(t[ps_in_ROI1_ymax].get_dataptr()),
                              static_cast<const float*>(t[ps_in_ROI1_x].get_dataptr()),
                              static_cast<const float*>(t[ps_in_ROI1_y].get_dataptr()),
                              static_cast<int32_t*>(t[ps_out_ROI1_time].get_dataptr()),
                              static_cast<int32_t*>(t[ps_out_ROI1_time_motion].get_dataptr()),
                              static_cast<const int32_t*>(t[ps_in_ROI1_prev_id].get_dataptr()),
                              static_cast<uint8_t*>(t[ps_out_ROI1_is_extrapolated].get_dataptr()),
                              n_ROI1,
                              trk.track_array->id, trk.track_array->begin, trk.track_array->end,
                              trk.track_array->extrapol_x, trk.track_array->extrapol_y, trk.track_array->state,
                              trk.track_array->obj_type, trk.track_array->change_state_reason,
                              &trk.track_array->_offset, &trk.track_array->_size, trk.BB_array,
                              frame,
                              *static_cast<double*>(t[ps_in_theta].get_dataptr()),
                              *static_cast<double*>(t[ps_in_tx].get_dataptr()),
                              *static_cast<double*>(t[ps_in_ty].get_dataptr()),
                              *static_cast<double*>(t[ps_in_mean_error].get_dataptr()),
                              *static_cast<double*>(t[ps_in_std_deviation].get_dataptr()),
                              trk.r_extrapol, trk.angle_max, trk.diff_dev, trk.track_all, trk.fra_star_min,
                              trk.fra_meteor_min, trk.fra_meteor_max);

            return aff3ct::module::status_t::SUCCESS;
        });
    }

    virtual ~Tracking() {
        tracking_free_data(this->tracking_data);
        tracking_free_track_array(this->track_array);
        tracking_free_BB_array(this->BB_array);
        free(this->BB_array);
    }

    track_t* get_track_array() {
        return this->track_array;
    }

    BB_t** get_BB_array() {
        return this->BB_array;
    }

    inline aff3ct::module::Task& operator[](const trk::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const trk::sck::perform s) {
        return aff3ct::module::Module::operator[]((size_t)trk::tsk::perform)[(size_t)s];
    }
};