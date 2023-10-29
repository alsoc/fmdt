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
    auto ps_out_n_RoIs = this->template create_socket_out<uint32_t>(p, "out_n_RoIs", 1);
    auto ps_out_RoIs_id = this->template create_socket_out<uint32_t>(p, "out_RoIs_id", max_RoIs_size);
    auto ps_out_RoIs_xmin = this->template create_socket_out<uint32_t>(p, "out_RoIs_xmin", max_RoIs_size);
    auto ps_out_RoIs_xmax = this->template create_socket_out<uint32_t>(p, "out_RoIs_xmax", max_RoIs_size);
    auto ps_out_RoIs_ymin = this->template create_socket_out<uint32_t>(p, "out_RoIs_ymin", max_RoIs_size);
    auto ps_out_RoIs_ymax = this->template create_socket_out<uint32_t>(p, "out_RoIs_ymax", max_RoIs_size);
    auto ps_out_RoIs_S = this->template create_socket_out<uint32_t>(p, "out_RoIs_S", max_RoIs_size);
    auto ps_out_RoIs_Sx = this->template create_socket_out<uint32_t>(p, "out_RoIs_Sx", max_RoIs_size);
    auto ps_out_RoIs_Sy = this->template create_socket_out<uint32_t>(p, "out_RoIs_Sy", max_RoIs_size);
    auto ps_out_RoIs_Sx2 = this->template create_socket_out<uint64_t>(p, "out_RoIs_Sx2", max_RoIs_size);
    auto ps_out_RoIs_Sy2 = this->template create_socket_out<uint64_t>(p, "out_RoIs_Sy2", max_RoIs_size);
    auto ps_out_RoIs_Sxy = this->template create_socket_out<uint64_t>(p, "out_RoIs_Sxy", max_RoIs_size);
    auto ps_out_RoIs_x = this->template create_socket_out<float>(p, "out_RoIs_x", max_RoIs_size);
    auto ps_out_RoIs_y = this->template create_socket_out<float>(p, "out_RoIs_y", max_RoIs_size);

    // if the CCL does not initialize the output img of labels, we need to do it the first time ;-)
    if (no_init_labels) {
        uint32_t* out_labels = static_cast<uint32_t*>(p[ps_out_labels].get_dataptr());
        std::fill(out_labels, out_labels + img_n_rows * img_n_cols, 0);
    }

    this->create_codelet(p, [ps_in_img, ps_out_labels, ps_out_n_RoIs, ps_out_RoIs_id, ps_out_RoIs_xmin,
                             ps_out_RoIs_xmax, ps_out_RoIs_ymin, ps_out_RoIs_ymax, ps_out_RoIs_S, ps_out_RoIs_Sx,
                             ps_out_RoIs_Sy, ps_out_RoIs_Sx2, ps_out_RoIs_Sy2, ps_out_RoIs_Sxy, ps_out_RoIs_x,
                             ps_out_RoIs_y]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lsl = static_cast<CCL_threshold_features&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint8_t** in_img = t[ps_in_img].get_2d_dataptr<const uint8_t>(lsl.b, lsl.b);
        uint32_t** out_labels = t[ps_out_labels].get_2d_dataptr<uint32_t>(lsl.b, lsl.b);

        uint32_t* m_out_n_ROI = static_cast<uint32_t*>(t[ps_out_n_RoIs].get_dataptr());
        *m_out_n_ROI = _CCL_threshold_features_apply(lsl.data, in_img, out_labels, lsl.threshold,
                                                     static_cast<uint32_t*>(t[ps_out_RoIs_id].get_dataptr()),
                                                     static_cast<uint32_t*>(t[ps_out_RoIs_xmin].get_dataptr()),
                                                     static_cast<uint32_t*>(t[ps_out_RoIs_xmax].get_dataptr()),
                                                     static_cast<uint32_t*>(t[ps_out_RoIs_ymin].get_dataptr()),
                                                     static_cast<uint32_t*>(t[ps_out_RoIs_ymax].get_dataptr()),
                                                     static_cast<uint32_t*>(t[ps_out_RoIs_S].get_dataptr()),
                                                     static_cast<uint32_t*>(t[ps_out_RoIs_Sx].get_dataptr()),
                                                     static_cast<uint32_t*>(t[ps_out_RoIs_Sy].get_dataptr()),
                                                     static_cast<uint64_t*>(t[ps_out_RoIs_Sx2].get_dataptr()),
                                                     static_cast<uint64_t*>(t[ps_out_RoIs_Sy2].get_dataptr()),
                                                     static_cast<uint64_t*>(t[ps_out_RoIs_Sxy].get_dataptr()),
                                                     static_cast<float*>(t[ps_out_RoIs_x].get_dataptr()),
                                                     static_cast<float*>(t[ps_out_RoIs_y].get_dataptr()),
                                                     lsl.max_RoIs_size, lsl.no_init_labels);
        assert(*m_out_n_ROI <= lsl.max_RoIs_size);
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
