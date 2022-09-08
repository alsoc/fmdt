#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

#include "fmdt/CCL.h"

namespace ccl {
    enum class tsk : size_t { apply, SIZE };
    namespace sck {
        enum class apply : size_t { in_img, out_img, out_n_ROI, out_data_er, out_data_era, out_data_rlc, out_data_eq,
                                    out_data_ner, status };
    }
}

class CCL_LSL : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    // CCL_data_t *data;
    const uint8_t** in_img;
    uint32_t** out_img;
    uint32_t** out_data_er;
    uint32_t** out_data_era;
    uint32_t** out_data_rlc;
public:
    CCL_LSL(const int i0, const int i1, const int j0, const int j1, const int b)
    : Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), /* data(nullptr), */ in_img(nullptr), out_img(nullptr),
      out_data_er(nullptr), out_data_era(nullptr), out_data_rlc(nullptr) {
        const std::string name = "CCL_LSL";
        this->set_name(name);
        this->set_short_name(name);

        this->init_data();

        const size_t i_socket_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);
        const size_t d_socket_size = ((i1 - i0) + 1) * ((j1 - j0) + 1);

        auto &p = this->create_task("apply");
        auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in_img", i_socket_size);
        auto ps_out_img = this->template create_socket_out<uint32_t>(p, "out_img", i_socket_size);
        auto ps_out_n_ROI = this->template create_socket_out<uint32_t>(p, "out_n_ROI", 1);

        auto ps_out_data_er = this->template create_socket_out<uint32_t>(p, "out_data_er", d_socket_size);
        auto ps_out_data_era = this->template create_socket_out<uint32_t>(p, "out_data_era", d_socket_size);
        auto ps_out_data_rlc = this->template create_socket_out<uint32_t>(p, "out_data_rlc", d_socket_size);
        auto ps_out_data_eq = this->template create_socket_out<uint32_t>(p, "out_data_eq", d_socket_size);
        auto ps_out_data_ner = this->template create_socket_out<uint32_t>(p, "out_data_ner", (i1 - i0) + 1);

        this->create_codelet(p, [ps_in_img, ps_out_img, ps_out_n_ROI, ps_out_data_er, ps_out_data_era, ps_out_data_rlc,
                                 ps_out_data_eq, ps_out_data_ner]
                             (aff3ct::module::Module &m, aff3ct::module::Task &t, const size_t frame_id) -> int {
            auto &lsl = static_cast<CCL_LSL&>(m);
            const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
            uint32_t* m_out_img = static_cast<uint32_t*>(t[ps_out_img].get_dataptr());
            uint32_t* m_out_data_er = static_cast<uint32_t*>(t[ps_out_data_er].get_dataptr());
            uint32_t* m_out_data_era = static_cast<uint32_t*>(t[ps_out_data_era].get_dataptr());
            uint32_t* m_out_data_rlc = static_cast<uint32_t*>(t[ps_out_data_rlc].get_dataptr());

            lsl.in_img[lsl.i0 - lsl.b] = m_in_img - (lsl.j0 - lsl.b);
            lsl.out_img[lsl.i0 - lsl.b] = m_out_img - (lsl.j0 - lsl.b);
            for (int i = lsl.i0 - lsl.b + 1; i <= lsl.i1 + lsl.b; i++) {
                lsl.out_img[i] = lsl.out_img[i - 1] + ((lsl.j1 - lsl.j0) + 1 + 2 * lsl.b);
                lsl.in_img[i] = lsl.in_img[i - 1] + ((lsl.j1 - lsl.j0) + 1 + 2 * lsl.b);
                lsl.out_data_er[i] = lsl.out_data_er[i - 1] + ((lsl.j1 - lsl.j0) + 1);
                lsl.out_data_era[i] = lsl.out_data_era[i - 1] + ((lsl.j1 - lsl.j0) + 1);
                lsl.out_data_rlc[i] = lsl.out_data_rlc[i - 1] + ((lsl.j1 - lsl.j0) + 1);
            }
            lsl.out_data_er[lsl.i0] = m_out_data_er - lsl.j0;
            lsl.out_data_era[lsl.i0] = m_out_data_era - lsl.j0;
            lsl.out_data_rlc[lsl.i0] = m_out_data_rlc - lsl.j0;
            for (int i = lsl.i0 + 1; i <= lsl.i1; i++) {
                lsl.out_data_er[i] = lsl.out_data_er[i - 1] + ((lsl.j1 - lsl.j0) + 1);
                lsl.out_data_era[i] = lsl.out_data_era[i - 1] + ((lsl.j1 - lsl.j0) + 1);
                lsl.out_data_rlc[i] = lsl.out_data_rlc[i - 1] + ((lsl.j1 - lsl.j0) + 1);
            }

            uint32_t* m_out_n_ROI = static_cast<uint32_t*>(t[ps_out_n_ROI].get_dataptr());
            *m_out_n_ROI = _CCL_LSL_apply(lsl.out_data_er,
                                          lsl.out_data_era,
                                          lsl.out_data_rlc,
                                          static_cast<uint32_t*>(t[ps_out_data_eq].get_dataptr()),
                                          static_cast<uint32_t*>(t[ps_out_data_ner].get_dataptr()),
                                          lsl.in_img, lsl.out_img, lsl.i0, lsl.i1, lsl.j0, lsl.j1);
            return aff3ct::module::status_t::SUCCESS;
        });
    }

    virtual ~CCL_LSL() {
        free(this->in_img + (this->i0 - this->b));
        free(this->out_img + (this->i0 - this->b));
        free(this->out_data_er + this->i0);
        free(this->out_data_era + this->i0);
        free(this->out_data_rlc + this->i0);
        // CCL_LSL_free_data(this->data);
    }

    virtual CCL_LSL* clone() const {
        auto m = new CCL_LSL(*this);
        m->deep_copy(*this);
        return m;
    }

    void deep_copy(const CCL_LSL &m)
    {
        Module::deep_copy(m);
        this->init_data();
    }

    inline uint32_t** get_out_img() {
        return this->out_img;
    }

    inline aff3ct::module::Task& operator[](const ccl::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const ccl::sck::apply s) {
        return aff3ct::module::Module::operator[]((size_t)ccl::tsk::apply)[(size_t)s];
    }

protected:
    void init_data() {
        // this->data = CCL_LSL_alloc_and_init_data(i0, i1, j0, j1);
        this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
        this->out_img = (uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint32_t*)));
        this->in_img -= i0 - b;
        this->out_img -= i0 - b;
        this->out_data_er = (uint32_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(uint32_t*)));
        this->out_data_era = (uint32_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(uint32_t*)));
        this->out_data_rlc = (uint32_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(uint32_t*)));
        this->out_data_er -= i0;
        this->out_data_era -= i0;
        this->out_data_rlc -= i0;
    }
};
