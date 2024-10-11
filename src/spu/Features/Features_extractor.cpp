#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/spu/Features/Features_extractor.hpp"

Features_extractor::Features_extractor(const int i0, const int i1, const int j0, const int j1, const int b,
                                       const size_t max_RoIs_size)
: spu::module::Stateful(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_RoIs_size(max_RoIs_size) {
    const std::string name = "Features_extractor";
    this->set_name(name);
    this->set_short_name(name);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("extract");
    auto ps_in_labels = this->template create_2d_socket_in<uint32_t>(p, "in_labels", img_n_rows, img_n_cols);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);
    auto ps_out_RoIs_basic = this->template create_socket_out<uint8_t>(p, "out_RoIs_basic",
                                                                       max_RoIs_size * sizeof(RoI_basic_t));

    this->create_codelet(p, [ps_in_labels, ps_in_n_RoIs, ps_out_RoIs_basic]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &ext = static_cast<Features_extractor&>(m);
        
        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint32_t**   in_labels      =  t[ps_in_labels     ].get_2d_dataptr<const uint32_t   >(ext.b, ext.b);
        const uint32_t     in_n_RoIs      = *t[ps_in_n_RoIs     ].get_dataptr   <const uint32_t   >();
              RoI_basic_t* out_RoIs_basic =  t[ps_out_RoIs_basic].get_dataptr   <      RoI_basic_t>();
        assert(in_n_RoIs <= ext.max_RoIs_size);

        features_extract(in_labels, ext.i0, ext.i1, ext.j0, ext.j1, out_RoIs_basic, in_n_RoIs);

        return spu::runtime::status_t::SUCCESS;
    });
}

Features_extractor::~Features_extractor() {
}

Features_extractor* Features_extractor::clone() const {
    auto m = new Features_extractor(*this);
    m->deep_copy(*this);
    return m;
}
