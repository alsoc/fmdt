#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

#include "fmdt/CCL.h"

namespace ccl {
    enum class tsk : size_t { apply, SIZE };
    namespace sck {
        enum class apply : size_t { in_img, out_img, out_n_ROI, status };
    }
}

class CCL_LSL : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    CCL_data_t *data;
    const uint8_t** in_img;
    uint32_t** out_img;
public:
    CCL_LSL(const int i0, const int i1, const int j0, const int j1, const int b)
    : Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), data(nullptr), in_img(nullptr), out_img(nullptr) {
        const std::string name = "CCL_LSL";
        this->set_name(name);
        this->set_short_name(name);

        this->data = CCL_LSL_alloc_and_init_data(i0, i1, j0, j1);
        this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
        this->out_img = (uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint32_t*)));
        this->in_img -= i0 - b;
        this->out_img -= i0 - b;

        auto socket_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

        auto &p = this->create_task("apply");
        auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in_img", socket_size);
        auto ps_out_img = this->template create_socket_out<uint32_t>(p, "out_img", socket_size);
        auto ps_out_n_ROI = this->template create_socket_out<uint32_t>(p, "out_n_ROI", 1);

        this->create_codelet(p, [ps_in_img, ps_out_img, ps_out_n_ROI](aff3ct::module::Module &m,
                             aff3ct::module::Task &t, const size_t frame_id) -> int {
            auto &lsl = static_cast<CCL_LSL&>(m);
            const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
            uint32_t* m_out_img = static_cast<uint32_t*>(t[ps_out_img].get_dataptr());
            lsl.in_img[lsl.i0 - lsl.b] = m_in_img - (lsl.j0 - lsl.b);
            lsl.out_img[lsl.i0 - lsl.b] = m_out_img - (lsl.j0 - lsl.b);
            for (int i = lsl.i0 - lsl.b + 1; i <= lsl.i1 + lsl.b; i++) {
                lsl.out_img[i] = lsl.out_img[i - 1] + ((lsl.j1 - lsl.j0) + 1 + 2 * lsl.b);
                lsl.in_img[i] = lsl.in_img[i - 1] + ((lsl.j1 - lsl.j0) + 1 + 2 * lsl.b);
            }

            uint32_t* m_out_n_ROI = static_cast<uint32_t*>(t[ps_out_n_ROI].get_dataptr());
            *m_out_n_ROI = CCL_LSL_apply(lsl.data, lsl.in_img, lsl.out_img, lsl.i0, lsl.i1, lsl.j0, lsl.j1);
            return aff3ct::module::status_t::SUCCESS;
        });
    }

    virtual ~CCL_LSL() {
        free(this->in_img + (i0 - b));
        free(this->out_img + (i0 - b));
        CCL_LSL_free_data(this->data);
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
};
