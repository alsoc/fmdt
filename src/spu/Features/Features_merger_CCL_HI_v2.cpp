#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/spu/Features/Features_merger_CCL_HI_v2.hpp"

Features_merger_CCL_HI_v2::Features_merger_CCL_HI_v2(const int i0, const int i1, const int j0, const int j1, const int b,
                                                     const uint32_t S_min, const uint32_t S_max,
                                                     const size_t max_in_RoIs_size, const size_t max_out_RoIs_size)
: spu::module::Stateful(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), S_min(S_min), S_max(S_max),
  max_in_RoIs_size(max_in_RoIs_size), max_out_RoIs_size(max_out_RoIs_size) {
    const std::string name = "Features_merger_CCL_HI_v2";
    this->set_name(name);
    this->set_short_name(name);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("merge");
    auto ps_in_labels = this->template create_2d_socket_in<uint32_t>(p, "in_labels", img_n_rows, img_n_cols);
    auto ps_in_img_HI = this->template create_2d_socket_in<uint8_t>(p, "in_img_HI", img_n_rows, img_n_cols);
    auto ps_fwd_RoIs_basic = this->template create_socket_fwd<uint8_t>(p, "fwd_RoIs_basic",
                                                                       max_in_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);
    auto ps_out_RoIs_basic = this->template create_socket_out<uint8_t>(p, "out_RoIs_basic",
                                                                       max_out_RoIs_size * sizeof(RoI_basic_t));
    auto ps_out_n_RoIs = this->template create_socket_out<uint32_t>(p, "out_n_RoIs", 1);
    auto ps_out_labels = this->template create_2d_socket_out<uint32_t>(p, "out_labels", img_n_rows, img_n_cols);

    this->create_codelet(p, [ps_in_labels, ps_in_img_HI, ps_fwd_RoIs_basic, ps_in_n_RoIs, ps_out_RoIs_basic,
                             ps_out_n_RoIs, ps_out_labels]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &mrg = static_cast<Features_merger_CCL_HI_v2&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint32_t**   in_labels      =  t[ps_in_labels     ].get_2d_dataptr<const uint32_t   >(mrg.b, mrg.b);
        const uint8_t**    in_img_HI      =  t[ps_in_img_HI     ].get_2d_dataptr<const uint8_t    >(mrg.b, mrg.b);
              uint32_t**   out_labels     =  t[ps_out_labels    ].get_2d_dataptr<      uint32_t   >(mrg.b, mrg.b);
              RoI_basic_t* fwd_RoIs_basic =  t[ps_fwd_RoIs_basic].get_dataptr   <      RoI_basic_t>();
        const uint32_t     in_n_RoIs      = *t[ps_in_n_RoIs     ].get_dataptr   <const uint32_t   >();
              RoI_basic_t* out_RoIs_basic =  t[ps_out_RoIs_basic].get_dataptr   <      RoI_basic_t>();
              uint32_t*    out_n_RoIs     =  t[ps_out_n_RoIs    ].get_dataptr   <      uint32_t   >();

        *out_n_RoIs = features_merge_CCL_HI_v2(in_labels, in_img_HI, out_labels, mrg.i0, mrg.i1, mrg.j0, mrg.j1,
                                              fwd_RoIs_basic, in_n_RoIs, mrg.S_min, mrg.S_max);
        assert(*out_n_RoIs <= mrg.max_out_RoIs_size);
        features_shrink(fwd_RoIs_basic, NULL, NULL, in_n_RoIs, out_RoIs_basic, NULL, NULL);

        return spu::runtime::status_t::SUCCESS;
    });
}

Features_merger_CCL_HI_v2::~Features_merger_CCL_HI_v2() {
}

Features_merger_CCL_HI_v2* Features_merger_CCL_HI_v2::clone() const {
    auto m = new Features_merger_CCL_HI_v2(*this);
    m->deep_copy(*this);
    return m;
}
