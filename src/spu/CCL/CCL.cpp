#include "fmdt/CCL/CCL_compute.h"
#include "fmdt/tools.h"

#include "fmdt/spu/CCL/CCL.hpp"

CCL::CCL(const int i0, const int i1, const int j0, const int j1, const int b, const enum ccl_impl_e impl)
: spu::module::Stateful(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), data(nullptr) {
    const std::string name = "CCL";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data(impl);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("apply");
    auto ps_in_img = this->template create_2d_socket_in<uint8_t>(p, "in_img", img_n_rows, img_n_cols);
    auto ps_out_labels = this->template create_2d_socket_out<uint32_t>(p, "out_labels", img_n_rows, img_n_cols);
    auto ps_out_n_RoIs = this->template create_socket_out<uint32_t>(p, "out_n_RoIs", 1);

    this->create_codelet(p, [ps_in_img, ps_out_labels, ps_out_n_RoIs]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &lsl = static_cast<CCL&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint8_t**  in_img     = t[ps_in_img    ].get_2d_dataptr<const uint8_t >(lsl.b, lsl.b);
              uint32_t** out_labels = t[ps_out_labels].get_2d_dataptr<      uint32_t>(lsl.b, lsl.b);
              uint32_t*  out_n_RoIs = t[ps_out_n_RoIs].get_dataptr   <      uint32_t>();

        *out_n_RoIs = CCL_apply(lsl.data, in_img, out_labels, 0);

        return spu::runtime::status_t::SUCCESS;
    });
}

void CCL::init_data(const enum ccl_impl_e impl) {
    this->data = CCL_alloc_data(impl, i0, i1, j0, j1);
    CCL_init_data(this->data);
}

CCL::~CCL() {
    CCL_free_data(this->data);
}

CCL* CCL::clone() const {
    auto m = new CCL(*this);
    m->deep_copy(*this);
    return m;
}

void CCL::deep_copy(const CCL &m) {
    spu::module::Stateful::deep_copy(m);
    this->init_data(this->data->impl);
}
