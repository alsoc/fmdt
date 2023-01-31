#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tracking/tracking_struct.h"

namespace trk {
    enum class tsk : size_t { perform, SIZE };
    namespace sck {
        enum class perform : size_t { in_frame, in_RoI_id, in_RoI_xmin, in_RoI_xmax, in_RoI_ymin, in_RoI_ymax, in_RoI_S,
                                      in_RoI_x, in_RoI_y, in_RoI_error, in_RoI_prev_id, in_RoI_magnitude,
                                      in_n_RoI, in_motion_est, status };
    }
}

class Tracking : public aff3ct::module::Module {
protected:
    const size_t r_extrapol;
    const float angle_max;
    const float diff_dev;
    const int track_all;
    const int magnitude;
    const size_t fra_star_min;
    const size_t fra_meteor_min;
    const size_t fra_meteor_max;
    const uint8_t extrapol_order_max;
    const float min_extrapol_ratio_S;
    const size_t max_RoI_size;
    tracking_data_t* tracking_data;
    vec_BB_t* BB_array;
public:
    Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
             const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
             const bool out_bb, const bool magnitude, const uint8_t extrapol_order_max,
             const float min_extrapol_ratio_S, const size_t max_RoI_size);
    virtual ~Tracking();
    inline tracking_data_t* get_data();
    inline vec_BB_t* get_BB_array();
    inline aff3ct::runtime::Task& operator[](const trk::tsk t);
    inline aff3ct::runtime::Socket& operator[](const trk::sck::perform s) ;

};

#include "fmdt/aff3ct_wrapper/Tracking/Tracking.hxx"
