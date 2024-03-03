#include "fmdt/motion/motion_compute.h"

#include "fmdt/aff3ct_wrapper/Motion/Motion.hpp"

Motion::Motion(const size_t max_RoIs_size)
: Module(), max_RoIs_size(max_RoIs_size) {
    const std::string name = "Motion";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("compute");

    auto ps_in_RoIs0_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs0_basic",
                                                                      max_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_RoIs1_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs1_basic",
                                                                      max_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_RoIs1_asso = this->template create_socket_in<uint8_t>(p, "in_RoIs1_asso",
                                                                     max_RoIs_size * sizeof(RoI_asso_t));
    auto ps_in_n_RoIs1 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs1", 1);
    auto ps_out_RoIs1_motion = this->template create_socket_out<uint8_t>(p, "out_RoIs1_motion",
                                                                         max_RoIs_size * sizeof(RoI_motion_t));

    auto ps_out_motion_est1 = this->template create_socket_out<uint8_t>(p, "out_motion_est1", sizeof(motion_t));
    auto ps_out_motion_est2 = this->template create_socket_out<uint8_t>(p, "out_motion_est2", sizeof(motion_t));

    this->create_codelet(p, [ps_in_RoIs0_basic, ps_in_RoIs1_basic, ps_in_RoIs1_asso, ps_in_n_RoIs1, ps_out_RoIs1_motion,
                             ps_out_motion_est1, ps_out_motion_est2]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {

        const RoI_basic_t*  in_RoIs0_basic   =  t[ps_in_RoIs0_basic  ].get_dataptr<const RoI_basic_t >();
        const RoI_basic_t*  in_RoIs1_basic   =  t[ps_in_RoIs1_basic  ].get_dataptr<const RoI_basic_t >();
        const RoI_asso_t*   in_RoIs1_asso    =  t[ps_in_RoIs1_asso   ].get_dataptr<const RoI_asso_t  >();
              RoI_motion_t* out_RoIs1_motion =  t[ps_out_RoIs1_motion].get_dataptr<      RoI_motion_t>();
        const uint32_t      in_n_RoIs1       = *t[ps_in_n_RoIs1      ].get_dataptr<const uint32_t    >();
              motion_t*     out_motion_est1  =  t[ps_out_motion_est1 ].get_dataptr<      motion_t    >();
              motion_t*     out_motion_est2  =  t[ps_out_motion_est2 ].get_dataptr<      motion_t    >();

        motion_compute(in_RoIs0_basic, in_RoIs1_basic, in_RoIs1_asso, out_RoIs1_motion, in_n_RoIs1, out_motion_est1,
                       out_motion_est2);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Motion::~Motion() {}

Motion* Motion::clone() const {
    auto m = new Motion(*this);
    m->deep_copy(*this);
    return m;
}
