#include "fmdt/CCL/CCL_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/CCL_LSL/CCL_LSL.hpp"

CCL_LSL::CCL_LSL(const int i0, const int i1, const int j0, const int j1, const int b)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), in_img(nullptr), out_labels(nullptr), out_data_er(nullptr),
  out_data_era(nullptr), out_data_rlc(nullptr) {
    const std::string name = "CCL_LSL";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data();

    const size_t i_socket_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);
    const size_t d_socket_size = ((i1 - i0) + 1) * ((j1 - j0) + 1);

    auto &p = this->create_task("apply");
    auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in_img", i_socket_size);
    auto ps_out_labels = this->template create_socket_out<uint32_t>(p, "out_labels", i_socket_size);
    auto ps_out_n_RoI = this->template create_socket_out<uint32_t>(p, "out_n_RoI", 1);

    auto ps_out_data_er = this->template create_socket_out<uint32_t>(p, "out_data_er", d_socket_size);
    auto ps_out_data_era = this->template create_socket_out<uint32_t>(p, "out_data_era", d_socket_size);
    auto ps_out_data_rlc = this->template create_socket_out<uint32_t>(p, "out_data_rlc", d_socket_size);
    auto ps_out_data_eq = this->template create_socket_out<uint32_t>(p, "out_data_eq", d_socket_size);
    auto ps_out_data_ner = this->template create_socket_out<uint32_t>(p, "out_data_ner", (i1 - i0) + 1);

    this->create_codelet(p, [ps_in_img, ps_out_labels, ps_out_n_RoI, ps_out_data_er, ps_out_data_era, ps_out_data_rlc,
                             ps_out_data_eq, ps_out_data_ner]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lsl = static_cast<CCL_LSL&>(m);
        const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
        uint32_t* m_out_labels = static_cast<uint32_t*>(t[ps_out_labels].get_dataptr());
        uint32_t* m_out_data_er = static_cast<uint32_t*>(t[ps_out_data_er].get_dataptr());
        uint32_t* m_out_data_era = static_cast<uint32_t*>(t[ps_out_data_era].get_dataptr());
        uint32_t* m_out_data_rlc = static_cast<uint32_t*>(t[ps_out_data_rlc].get_dataptr());

        tools_linear_2d_nrc_ui8matrix(m_in_img, lsl.i0 - lsl.b, lsl.i1 + lsl.b, lsl.j0 - lsl.b, lsl.j1 + lsl.b, 
                                      lsl.in_img);
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_out_labels, lsl.i0 - lsl.b, lsl.i1 + lsl.b, lsl.j0 - lsl.b,
                                       lsl.j1 + lsl.b, (const uint32_t**)lsl.out_labels);
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_out_data_er, lsl.i0, lsl.i1, lsl.j0, lsl.j1,
                                       (const uint32_t**)lsl.out_data_er);
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_out_data_era, lsl.i0, lsl.i1, lsl.j0, lsl.j1,   
                                       (const uint32_t**)lsl.out_data_era);
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_out_data_rlc, lsl.i0, lsl.i1, lsl.j0, lsl.j1,   
                                       (const uint32_t**)lsl.out_data_rlc);

        uint32_t* m_out_n_RoI = static_cast<uint32_t*>(t[ps_out_n_RoI].get_dataptr());
        *m_out_n_RoI = _CCL_LSL_apply(lsl.out_data_er,
                                      lsl.out_data_era,
                                      lsl.out_data_rlc,
                                      static_cast<uint32_t*>(t[ps_out_data_eq].get_dataptr()),
                                      static_cast<uint32_t*>(t[ps_out_data_ner].get_dataptr()),
                                      lsl.in_img, lsl.out_labels, lsl.i0, lsl.i1, lsl.j0, lsl.j1);
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

void CCL_LSL::init_data() {
    // this->data = CCL_LSL_alloc_and_init_data(i0, i1, j0, j1);
    this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
    this->out_labels = (uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint32_t*)));
    this->in_img -= i0 - b;
    this->out_labels -= i0 - b;
    this->out_data_er = (uint32_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(uint32_t*)));
    this->out_data_era = (uint32_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(uint32_t*)));
    this->out_data_rlc = (uint32_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(uint32_t*)));
    this->out_data_er -= i0;
    this->out_data_era -= i0;
    this->out_data_rlc -= i0;
}

CCL_LSL::~CCL_LSL() {
    free(this->in_img + (this->i0 - this->b));
    free(this->out_labels + (this->i0 - this->b));
    free(this->out_data_er + this->i0);
    free(this->out_data_era + this->i0);
    free(this->out_data_rlc + this->i0);
}

CCL_LSL* CCL_LSL::clone() const {
    auto m = new CCL_LSL(*this);
    m->deep_copy(*this);
    return m;
}

void CCL_LSL::deep_copy(const CCL_LSL &m)
{
    Module::deep_copy(m);
    this->init_data();
}
