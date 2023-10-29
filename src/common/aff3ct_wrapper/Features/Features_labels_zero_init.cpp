#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Features/Features_labels_zero_init.hpp"

Features_labels_zero_init::Features_labels_zero_init(const int i0, const int i1, const int j0, const int j1, const int b,
                                                     const size_t max_in_RoIs_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_in_RoIs_size(max_in_RoIs_size) {
    const std::string name = "Features_labels_zero_init";
    this->set_name(name);
    this->set_short_name(name);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("zinit");
    auto ps_fwd_labels = this->template create_2d_socket_fwd<uint32_t>(p, "in_out_labels", img_n_rows, img_n_cols);
    auto ps_in_RoIs_xmin = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmin", max_in_RoIs_size);
    auto ps_in_RoIs_xmax = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmax", max_in_RoIs_size);
    auto ps_in_RoIs_ymin = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymin", max_in_RoIs_size);
    auto ps_in_RoIs_ymax = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymax", max_in_RoIs_size);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);
    // this is a hack to lauch this task after the tracking (zinit[in_fake] = mergev3[status])
    /* auto ps_in_fake = */ this->template create_socket_in<int32_t>(p, "in_fake", 1);

    this->create_codelet(p, [ps_fwd_labels, ps_in_RoIs_xmin, ps_in_RoIs_xmax, ps_in_RoIs_ymin, ps_in_RoIs_ymax,
                             ps_in_n_RoIs]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lzi = static_cast<Features_labels_zero_init&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        uint32_t** fwd_labels = t[ps_fwd_labels].get_2d_dataptr<uint32_t>(lzi.b, lzi.b);

        _features_labels_zero_init(static_cast<const uint32_t*>(t[ps_in_RoIs_xmin].get_dataptr()),
                                   static_cast<const uint32_t*>(t[ps_in_RoIs_xmax].get_dataptr()),
                                   static_cast<const uint32_t*>(t[ps_in_RoIs_ymin].get_dataptr()),
                                   static_cast<const uint32_t*>(t[ps_in_RoIs_ymax].get_dataptr()),
                                   *static_cast<const uint32_t*>(t[ps_in_n_RoIs].get_dataptr()),
                                   fwd_labels);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Features_labels_zero_init::~Features_labels_zero_init() {
}

Features_labels_zero_init* Features_labels_zero_init::clone() const {
    auto m = new Features_labels_zero_init(*this);
    m->deep_copy(*this);
    return m;
}
