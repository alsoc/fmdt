#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Features/Features_magnitude.hpp"

Features_magnitude::Features_magnitude(const int i0, const int i1, const int j0, const int j1, const int b,
                                       const size_t max_RoIs_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_RoIs_size(max_RoIs_size) {
    const std::string name = "Features_magnitude";
    this->set_name(name);
    this->set_short_name(name);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("compute");
    auto ps_in_img = this->template create_2d_socket_in<uint8_t>(p, "in_img", img_n_rows, img_n_cols);
    auto ps_in_labels = this->template create_2d_socket_in<uint32_t>(p, "in_labels", img_n_rows, img_n_cols);

    auto ps_in_RoIs_xmin = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmin", max_RoIs_size);
    auto ps_in_RoIs_xmax = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmax", max_RoIs_size);
    auto ps_in_RoIs_ymin = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymin", max_RoIs_size);
    auto ps_in_RoIs_ymax = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymax", max_RoIs_size);
    auto ps_in_RoIs_S = this->template create_socket_in<uint32_t>(p, "in_RoIs_S", max_RoIs_size);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);

    auto ps_out_RoIs_magnitude = this->template create_socket_out<uint32_t>(p, "out_RoIs_magnitude", max_RoIs_size);
    auto ps_out_RoIs_sat_count = this->template create_socket_out<uint32_t>(p, "out_RoIs_sat_count", max_RoIs_size);

    this->create_codelet(p, [ps_in_img, ps_in_labels, ps_in_RoIs_xmin, ps_in_RoIs_xmax, ps_in_RoIs_ymin,
                             ps_in_RoIs_ymax, ps_in_RoIs_S, ps_in_n_RoIs, ps_out_RoIs_magnitude, ps_out_RoIs_sat_count]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &mgn = static_cast<Features_magnitude&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint8_t** in_img = t[ps_in_img].get_2d_dataptr<const uint8_t>(mgn.b, mgn.b);
        const uint32_t** in_labels = t[ps_in_labels].get_2d_dataptr<const uint32_t>(mgn.b, mgn.b);

        const uint32_t in_n_RoIs = *static_cast<const uint32_t*>(t[ps_in_n_RoIs].get_dataptr());

        _features_compute_magnitude(in_img,
                                    mgn.i0,
                                    mgn.i1,
                                    mgn.j0,
                                    mgn.j1,
                                    in_labels,
                                    static_cast<const uint32_t*>(t[ps_in_RoIs_xmin].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_RoIs_xmax].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_RoIs_ymin].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_RoIs_ymax].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_RoIs_S].get_dataptr()),
                                    static_cast<uint32_t*>(t[ps_out_RoIs_magnitude].get_dataptr()),
                                    static_cast<uint32_t*>(t[ps_out_RoIs_sat_count].get_dataptr()),
                                    in_n_RoIs);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Features_magnitude::~Features_magnitude() {
}

Features_magnitude* Features_magnitude::clone() const {
    auto m = new Features_magnitude(*this);
    m->deep_copy(*this);
    return m;
}
