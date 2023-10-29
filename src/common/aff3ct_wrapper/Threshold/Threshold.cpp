#include "fmdt/threshold/threshold_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Threshold/Threshold.hpp"

Threshold::Threshold(const int i0, const int i1, const int j0, const int j1, const int b, const uint8_t thr_val)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), thr_val(thr_val) {
    const std::string name = "Threshold";
    this->set_name(name);
    this->set_short_name(name);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("apply");
    auto ps_in_img = this->template create_2d_socket_in<uint8_t>(p, "in", img_n_rows, img_n_cols);
    auto ps_out_img = this->template create_2d_socket_out<uint8_t>(p, "out", img_n_rows, img_n_cols);

    this->create_codelet(p, [ps_in_img, ps_out_img](aff3ct::module::Module &m, aff3ct::runtime::Task &t,
                         const size_t frame_id) -> int {
        auto &thr = static_cast<Threshold&>(m);
        
        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint8_t** in_img = t[ps_in_img].get_2d_dataptr<const uint8_t>(thr.b, thr.b);
        uint8_t** out_img = t[ps_out_img].get_2d_dataptr<uint8_t>(thr.b, thr.b);

        threshold(in_img, out_img, thr.i0, thr.i1, thr.j0, thr.j1, thr.thr_val);
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Threshold::~Threshold() {
}

Threshold* Threshold::clone() const {
    auto m = new Threshold(*this);
    m->deep_copy(*this);
    return m;
}
