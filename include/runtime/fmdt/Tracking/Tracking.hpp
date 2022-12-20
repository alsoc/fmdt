#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tracking.h"

namespace trk {
    enum class tsk : size_t { perform, SIZE };
    namespace sck {
        enum class perform : size_t { in_frame, in_ROI_id, in_ROI_xmin, in_ROI_xmax, in_ROI_ymin, in_ROI_ymax,
                                      in_ROI_x, in_ROI_y, in_ROI_error, in_ROI_prev_id, in_ROI_magnitude,
                                      in_n_ROI, in_motion_est, status };
    }
}

class Tracking : public aff3ct::module::Module {
protected:
    const size_t r_extrapol;
    const float angle_max;
    const float diff_dev;
    const int track_all;
    const int magnitude;
    const uint8_t extrapol_order_max;
    const size_t fra_star_min;
    const size_t fra_meteor_min;
    const size_t fra_meteor_max;
    const size_t max_ROI_size;
    tracking_data_t* tracking_data;
    vec_BB_t* BB_array;
public:
    Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
             const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
             const bool out_bb, const bool magnitude, const uint8_t extrapol_order_max, const size_t max_ROI_size);
    virtual ~Tracking();
    inline tracking_data_t* get_data();
    inline vec_BB_t* get_BB_array();
    inline aff3ct::runtime::Task& operator[](const trk::tsk t);
    inline aff3ct::runtime::Socket& operator[](const trk::sck::perform s) ;

};

#include "fmdt/Tracking/Tracking.hxx"
