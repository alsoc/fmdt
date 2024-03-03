/*!
 * \file
 * \brief C++ wrapper to compute tracking.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tracking/tracking_struct.h"

namespace trk {
    enum class tsk : size_t { perform, perform_elli, SIZE };
    namespace sck {
        enum class perform : size_t { in_frame, in_RoIs_basic, in_RoIs_asso, in_RoIs_motion, in_n_RoIs, in_motion_est,
                                      status };
        enum class perform_elli : size_t { in_frame, in_RoIs_basic, in_RoIs_asso, in_RoIs_motion, in_RoIs_elli,
                                           in_n_RoIs, in_motion_est, status };
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
    const float min_ellipse_ratio;
    const size_t max_RoIs_size;
    tracking_data_t* tracking_data;
public:
    Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
             const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
             const bool save_RoIs_id, const uint8_t extrapol_order_max, const float min_extrapol_ratio_S,
             const float min_ellipse_ratio, const size_t max_RoIs_size);
    virtual ~Tracking();
    inline tracking_data_t* get_data();
    inline aff3ct::runtime::Task& operator[](const trk::tsk t);
    inline aff3ct::runtime::Socket& operator[](const trk::sck::perform s) ;
    inline aff3ct::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/aff3ct_wrapper/Tracking/Tracking.hxx"
