#include "fmdt/threshold.h"
#include "fmdt/tools.h"

#include "fmdt/Threshold/Threshold.hpp"

Threshold::Threshold(const int i0, const int i1, const int j0, const int j1, const int b, const uint8_t thr_val)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), thr_val(thr_val), in_img(nullptr), out_img(nullptr) {
    const std::string name = "Threshold";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data();

    auto socket_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("apply");
    auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in", socket_size);
    auto ps_out_img = this->template create_socket_out<uint8_t>(p, "out", socket_size);

    this->create_codelet(p, [ps_in_img, ps_out_img](aff3ct::module::Module &m, aff3ct::runtime::Task &t,
                         const size_t frame_id) -> int {
        auto &thr = static_cast<Threshold&>(m);
        
        const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
        tools_linear_2d_nrc_ui8matrix(m_in_img, thr.i0 - thr.b, thr.i1 + thr.b, thr.j0 - thr.b, thr.j1 + thr.b,  
                                      thr.in_img);

        uint8_t* m_out_img = static_cast<uint8_t*>(t[ps_out_img].get_dataptr());
        tools_linear_2d_nrc_ui8matrix((const uint8_t*)m_out_img, thr.i0 - thr.b, thr.i1 + thr.b, thr.j0 - thr.b, 
                                      thr.j1 + thr.b, (const uint8_t**)thr.out_img);

        threshold(thr.in_img, thr.out_img, thr.i0, thr.i1, thr.j0, thr.j1, thr.thr_val);
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

void Threshold::init_data() {
    this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
    this->out_img = (uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint8_t*)));
    this->in_img -= i0 - b;
    this->out_img -= i0 - b;
}

Threshold::~Threshold() {
    free(this->in_img + this->i0 - this->b);
    free(this->out_img + this->i0 - this->b);
}

Threshold* Threshold::clone() const {
    auto m = new Threshold(*this);
    m->deep_copy(*this);
    return m;
}

void Threshold::deep_copy(const Threshold &m)
{
    Module::deep_copy(m);
    this->init_data();
}
