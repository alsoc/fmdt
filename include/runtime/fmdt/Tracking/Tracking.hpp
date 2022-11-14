#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tracking.h"

namespace trk {
    enum class tsk : size_t { perform, SIZE };
    namespace sck {
        enum class perform : size_t { in_frame, in_ROI0_id, in_ROI0_xmin, in_ROI0_xmax, in_ROI0_ymin, in_ROI0_ymax,
                                      in_ROI0_x, in_ROI0_y, in_ROI0_error, in_ROI0_next_id, in_n_ROI0, in_ROI1_id, 
                                      in_ROI1_xmin, in_ROI1_xmax, in_ROI1_ymin, in_ROI1_ymax, in_ROI1_x, in_ROI1_y, 
                                      in_ROI1_prev_id, in_n_ROI1, in_theta, in_tx, in_ty, in_mean_error, 
                                      in_std_deviation, out_track_id, out_track_begin, out_track_end,
                                      out_track_extrapol_x, out_track_extrapol_y, out_track_state, out_track_obj_type,
                                      out_track_change_state_reason, out_n_tracks, status };
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
    const size_t max_bb_list_size;
    int32_t* ROI0_prev_id;
    tracking_data_t* tracking_data;
    track_t* track_array;
    BB_t** BB_array;
public:
    Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
             const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
             const bool out_bb, const size_t max_ROI_size, const size_t max_tracks_size, const size_t max_bb_list_size);
    virtual ~Tracking();
    inline track_t* get_track_array();
    inline BB_t** get_BB_array();
    inline int32_t* get_ROI0_prev_id();
    inline aff3ct::runtime::Task& operator[](const trk::tsk t);
    inline aff3ct::runtime::Socket& operator[](const trk::sck::perform s) ;

};

#include "fmdt/Tracking/Tracking.hxx"
