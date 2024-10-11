#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/spu/Features/Features_labels_zero_init.hpp"

Features_labels_zero_init::Features_labels_zero_init(const int i0, const int i1, const int j0, const int j1, const int b,
                                                     const size_t max_in_RoIs_size)
: spu::module::Stateful(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_in_RoIs_size(max_in_RoIs_size) {
    const std::string name = "Features_labels_zero_init";
    this->set_name(name);
    this->set_short_name(name);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("zinit");
    auto ps_in_RoIs_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs_basic",
                                                                     max_in_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);
    auto ps_fwd_labels = this->template create_2d_socket_fwd<uint32_t>(p, "fwd_labels", img_n_rows, img_n_cols);

    this->create_codelet(p, [ps_in_RoIs_basic, ps_in_n_RoIs, ps_fwd_labels]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &lzi = static_cast<Features_labels_zero_init&>(m);

        const RoI_basic_t* in_RoIs_basic =  t[ps_in_RoIs_basic].get_dataptr   <const RoI_basic_t>();
        const uint32_t     in_n_RoIs     = *t[ps_in_n_RoIs    ].get_dataptr   <const uint32_t   >();
        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
              uint32_t**   fwd_labels    =  t[ps_fwd_labels   ].get_2d_dataptr<      uint32_t   >(lzi.b, lzi.b);

        features_labels_zero_init(in_RoIs_basic, in_n_RoIs, fwd_labels);

        return spu::runtime::status_t::SUCCESS;
    });
}

Features_labels_zero_init::~Features_labels_zero_init() {
}

Features_labels_zero_init* Features_labels_zero_init::clone() const {
    auto m = new Features_labels_zero_init(*this);
    m->deep_copy(*this);
    return m;
}
