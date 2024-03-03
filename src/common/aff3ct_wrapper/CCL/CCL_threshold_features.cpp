#include "fmdt/CCL/CCL_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/CCL/CCL_threshold_features.hpp"

CCL_threshold_features::CCL_threshold_features(const int i0, const int i1, const int j0, const int j1, const int b,
                                               const uint8_t threshold, const size_t max_RoIs_size,
                                               const enum ccl_impl_e impl, const bool no_init_labels)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), threshold(threshold), max_RoIs_size(max_RoIs_size), data(nullptr),
  no_init_labels(no_init_labels) {
    const std::string name = "CCL_threshold_features";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data(impl);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("apply");
    auto ps_in_img = this->template create_2d_socket_in<uint8_t>(p, "in_img", img_n_rows, img_n_cols);
    auto ps_out_labels = this->template create_2d_socket_out<uint32_t>(p, "out_labels", img_n_rows, img_n_cols);
    auto ps_out_RoIs_basic = this->template create_socket_out<uint8_t>(p, "out_RoIs_basic",
                                                                       max_RoIs_size * sizeof(RoI_basic_t));
    auto ps_out_n_RoIs = this->template create_socket_out<uint32_t>(p, "out_n_RoIs", 1);

    // if the CCL does not initialize the output img of labels, we need to do it the first time ;-)
    if (no_init_labels) {
        uint32_t* out_labels = static_cast<uint32_t*>(p[ps_out_labels].get_dataptr());
        std::fill(out_labels, out_labels + img_n_rows * img_n_cols, 0);
    }

    this->create_codelet(p, [ps_in_img, ps_out_labels, ps_out_RoIs_basic, ps_out_n_RoIs]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lsl = static_cast<CCL_threshold_features&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint8_t**    in_img         = t[ps_in_img        ].get_2d_dataptr<const uint8_t    >(lsl.b, lsl.b);
              uint32_t**   out_labels     = t[ps_out_labels    ].get_2d_dataptr<      uint32_t   >(lsl.b, lsl.b);
              RoI_basic_t* out_RoIs_basic = t[ps_out_RoIs_basic].get_dataptr   <      RoI_basic_t>();
              uint32_t*    out_n_RoIs     = t[ps_out_n_RoIs    ].get_dataptr   <      uint32_t   >();

        *out_n_RoIs = CCL_threshold_features_apply(lsl.data, in_img, out_labels, lsl.threshold, out_RoIs_basic,
                                                   lsl.max_RoIs_size, lsl.no_init_labels);
        assert(*out_n_RoIs <= lsl.max_RoIs_size);
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

void CCL_threshold_features::init_data(const enum ccl_impl_e impl) {
    this->data = CCL_alloc_data(impl, i0, i1, j0, j1);
    CCL_init_data(this->data);
}

CCL_threshold_features::~CCL_threshold_features() {
    CCL_free_data(this->data);
}

CCL_threshold_features* CCL_threshold_features::clone() const {
    auto m = new CCL_threshold_features(*this);
    m->deep_copy(*this);
    return m;
}

void CCL_threshold_features::deep_copy(const CCL_threshold_features &m) {
    Module::deep_copy(m);
    this->init_data(this->data->impl);
}
