/*!
 * \file
 * \brief C++ wrapper to compute tracking.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tracking/tracking_struct.h"

namespace trk {
    enum class tsk : size_t { perform, SIZE };
    namespace sck {
        enum class perform : size_t { in_frame, in_RoIs_id, in_RoIs_xmin, in_RoIs_xmax, in_RoIs_ymin, in_RoIs_ymax,
                                      in_RoIs_S, in_RoIs_x, in_RoIs_y, in_RoIs_error, in_RoIs_prev_id, in_n_RoIs,
                                      in_motion_est, status };
    }
}

class Tracking : public aff3ct::module::Module {
protected:
    const size_t r_extrapol;
    const float angle_max;
    const float diff_dev;
    const int track_all;
    const bool save_RoIs_id;
    const size_t fra_star_min;
    const size_t fra_meteor_min;
    const size_t fra_meteor_max;
    const uint8_t extrapol_order_max;
    const float min_extrapol_ratio_S;
    const size_t max_RoIs_size;
    tracking_data_t* tracking_data;
public:
    Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
             const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
             const bool save_RoIs_id, const uint8_t extrapol_order_max, const float min_extrapol_ratio_S,
             const size_t max_RoIs_size);
    virtual ~Tracking();
    inline tracking_data_t* get_data();
    inline aff3ct::runtime::Task& operator[](const trk::tsk t);
    inline aff3ct::runtime::Socket& operator[](const trk::sck::perform s) ;
    inline aff3ct::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/aff3ct_wrapper/Tracking/Tracking.hxx"
