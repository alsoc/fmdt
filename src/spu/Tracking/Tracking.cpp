#include "fmdt/tracking/tracking_compute.h"
#include "vec.h"

#include "fmdt/spu/Tracking/Tracking.hpp"

Tracking::Tracking(const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
                   const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
                   const bool save_RoIs_id, const uint8_t extrapol_order_max, const float min_extrapol_ratio_S,
                   const float min_ellipse_ratio, const size_t max_RoIs_size)
: spu::module::Stateful(), r_extrapol(r_extrapol), angle_max(angle_max), diff_dev(diff_dev), track_all(track_all),
  save_RoIs_id(save_RoIs_id), fra_star_min(fra_star_min), fra_meteor_min(fra_meteor_min),
  fra_meteor_max(fra_meteor_max), extrapol_order_max(extrapol_order_max), min_extrapol_ratio_S(min_extrapol_ratio_S),
  min_ellipse_ratio(min_ellipse_ratio), max_RoIs_size(max_RoIs_size), tracking_data(nullptr) {
    const std::string name = "Tracking";
    this->set_name(name);
    this->set_short_name(name);

    this->tracking_data = tracking_alloc_data(std::max(fra_star_min, fra_meteor_min), max_RoIs_size);

    auto &p1 = this->create_task("perform");

    auto p1s_in_frame = this->template create_socket_in<uint32_t>(p1, "in_frame", 1);
    auto p1s_in_RoIs_basic = this->template create_socket_in<uint8_t>(p1, "in_RoIs_basic",
                                                                      max_RoIs_size * sizeof(RoI_basic_t));
    auto p1s_in_RoIs_asso = this->template create_socket_in<uint8_t>(p1, "in_RoIs_asso",
                                                                     max_RoIs_size * sizeof(RoI_asso_t));
    auto p1s_in_RoIs_motion = this->template create_socket_in<uint8_t>(p1, "in_RoIs_motion",
                                                                       max_RoIs_size * sizeof(RoI_motion_t));
    auto p1s_in_n_RoIs = this->template create_socket_in<uint32_t>(p1, "in_n_RoIs", 1);
    auto p1s_in_motion_est = this->template create_socket_in<uint8_t>(p1, "in_motion_est", sizeof(motion_t));

    this->create_codelet(p1, [p1s_in_frame, p1s_in_RoIs_basic, p1s_in_RoIs_asso, p1s_in_RoIs_motion, p1s_in_n_RoIs,
                              p1s_in_motion_est]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &trk = static_cast<Tracking&>(m);

        const RoI_basic_t*  in_RoIs_basic  =  t[p1s_in_RoIs_basic ].get_dataptr<const RoI_basic_t >();
        const RoI_asso_t*   in_RoIs_asso   =  t[p1s_in_RoIs_asso  ].get_dataptr<const RoI_asso_t  >();
        const RoI_motion_t* in_RoIs_motion =  t[p1s_in_RoIs_motion].get_dataptr<const RoI_motion_t>();
        const uint32_t      in_n_RoIs      = *t[p1s_in_n_RoIs     ].get_dataptr<const uint32_t    >();
        const uint32_t      in_frame       = *t[p1s_in_frame      ].get_dataptr<const uint32_t    >();
        const motion_t*     in_motion_est  =  t[p1s_in_motion_est ].get_dataptr<const motion_t    >();

        RoIs_t the_RoIs = { (RoI_basic_t*)in_RoIs_basic,
                            (RoI_asso_t*)in_RoIs_asso,
                            (RoI_motion_t*)in_RoIs_motion,
                            nullptr,
                            nullptr,
                            in_n_RoIs,
                            trk.max_RoIs_size };

        tracking_perform(trk.tracking_data, &the_RoIs, in_frame, in_motion_est,
                         trk.r_extrapol, trk.angle_max, trk.diff_dev, trk.track_all, trk.fra_star_min,
                         trk.fra_meteor_min, trk.fra_meteor_max, trk.save_RoIs_id, trk.extrapol_order_max,
                         trk.min_extrapol_ratio_S, trk.min_ellipse_ratio);

        return spu::runtime::status_t::SUCCESS;
    });


    auto &p2 = this->create_task("perform_elli");

    auto p2s_in_frame = this->template create_socket_in<uint32_t>(p2, "in_frame", 1);
    auto p2s_in_RoIs_basic = this->template create_socket_in<uint8_t>(p2, "in_RoIs_basic",
                                                                      max_RoIs_size * sizeof(RoI_basic_t));
    auto p2s_in_RoIs_asso = this->template create_socket_in<uint8_t>(p2, "in_RoIs_asso",
                                                                     max_RoIs_size * sizeof(RoI_asso_t));
    auto p2s_in_RoIs_motion = this->template create_socket_in<uint8_t>(p2, "in_RoIs_motion",
                                                                       max_RoIs_size * sizeof(RoI_motion_t));
    auto p2s_in_RoIs_elli = this->template create_socket_in<uint8_t>(p2, "in_RoIs_elli",
                                                                     max_RoIs_size * sizeof(RoI_elli_t));
    auto p2s_in_n_RoIs = this->template create_socket_in<uint32_t>(p2, "in_n_RoIs", 1);
    auto p2s_in_motion_est = this->template create_socket_in<uint8_t>(p2, "in_motion_est", sizeof(motion_t));

    this->create_codelet(p2, [p2s_in_frame, p2s_in_RoIs_basic, p2s_in_RoIs_asso, p2s_in_RoIs_motion, p2s_in_RoIs_elli,
                              p2s_in_n_RoIs, p2s_in_motion_est]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &trk = static_cast<Tracking&>(m);

        const RoI_basic_t*  in_RoIs_basic  =  t[p2s_in_RoIs_basic ].get_dataptr<const RoI_basic_t >();
        const RoI_asso_t*   in_RoIs_asso   =  t[p2s_in_RoIs_asso  ].get_dataptr<const RoI_asso_t  >();
        const RoI_motion_t* in_RoIs_motion =  t[p2s_in_RoIs_motion].get_dataptr<const RoI_motion_t>();
        const RoI_elli_t*   in_RoIs_elli   =  t[p2s_in_RoIs_elli  ].get_dataptr<const RoI_elli_t  >();
        const uint32_t      in_n_RoIs      = *t[p2s_in_n_RoIs     ].get_dataptr<const uint32_t    >();
        const uint32_t      in_frame       = *t[p2s_in_frame      ].get_dataptr<const uint32_t    >();
        const motion_t*     in_motion_est  =  t[p2s_in_motion_est ].get_dataptr<const motion_t    >();

        RoIs_t the_RoIs = { (RoI_basic_t*)in_RoIs_basic,
                            (RoI_asso_t*)in_RoIs_asso,
                            (RoI_motion_t*)in_RoIs_motion,
                            nullptr,
                            (RoI_elli_t*)in_RoIs_elli,
                            in_n_RoIs,
                            trk.max_RoIs_size };

        tracking_perform(trk.tracking_data, &the_RoIs, in_frame, in_motion_est,
                         trk.r_extrapol, trk.angle_max, trk.diff_dev, trk.track_all, trk.fra_star_min,
                         trk.fra_meteor_min, trk.fra_meteor_max, trk.save_RoIs_id, trk.extrapol_order_max,
                         trk.min_extrapol_ratio_S, trk.min_ellipse_ratio);

        return spu::runtime::status_t::SUCCESS;
    });
}

Tracking::~Tracking() {
    tracking_free_data(this->tracking_data);
}
