#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/spu/Features/Features_ellipse.hpp"

Features_ellipse::Features_ellipse(const size_t max_RoIs_size)
: spu::module::Stateful(), max_RoIs_size(max_RoIs_size) {
    const std::string name = "Features_ellipse";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("compute");

    auto ps_in_RoIs_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs_basic",
                                                                     max_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);
    auto ps_out_RoIs_elli = this->template create_socket_out<uint8_t>(p, "out_RoIs_elli",
                                                                      max_RoIs_size * sizeof(RoI_elli_t));

    this->create_codelet(p, [ps_in_RoIs_basic, ps_in_n_RoIs, ps_out_RoIs_elli]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        // auto &ell = static_cast<Features_ellipse&>(m);

        const RoI_basic_t* in_RoIs_basic =  t[ps_in_RoIs_basic].get_dataptr<const RoI_basic_t>();
        const uint32_t     in_n_RoIs     = *t[ps_in_n_RoIs    ].get_dataptr<const uint32_t   >();
              RoI_elli_t*  out_RoIs_elli =  t[ps_out_RoIs_elli].get_dataptr<      RoI_elli_t >();

        features_compute_ellipse(in_RoIs_basic, out_RoIs_elli, in_n_RoIs);

        return spu::runtime::status_t::SUCCESS;
    });
}

Features_ellipse* Features_ellipse::clone() const {
    auto m = new Features_ellipse(*this);
    m->deep_copy(*this);
    return m;
}
