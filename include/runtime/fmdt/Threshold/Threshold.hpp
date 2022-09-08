#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

#include "fmdt/threshold.h"

namespace thr {
    enum class tsk : size_t { apply, SIZE };
    namespace sck {
        enum class apply : size_t { in_img, out_img, status };
    }
}

class Threshold : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const uint8_t thr_val;
    const uint8_t** in_img;
    uint8_t** out_img;
public:
    Threshold(const int i0, const int i1, const int j0, const int j1, const int b, const uint8_t thr_val)
    : Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), thr_val(thr_val), in_img(nullptr), out_img(nullptr) {
        const std::string name = "Threshold";
        this->set_name(name);
        this->set_short_name(name);

        this->init_data();

        auto socket_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

        auto &p = this->create_task("apply");
        auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in", socket_size);
        auto ps_out_img = this->template create_socket_out<uint8_t>(p, "out", socket_size);

        this->create_codelet(p, [ps_in_img, ps_out_img](aff3ct::module::Module &m, aff3ct::module::Task &t,
                             const size_t frame_id) -> int {
            auto &thr = static_cast<Threshold&>(m);
            const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
            thr.in_img[thr.i0 - thr.b] = m_in_img - thr.j0;
            for (int i = thr.i0 - thr.b + 1; i <= thr.i1 + thr.b; i++)
                thr.in_img[i] = thr.in_img[i - 1] + ((thr.j1 - thr.j0) + 1 + 2 * thr.b);

            uint8_t* m_out_img = static_cast<uint8_t*>(t[ps_out_img].get_dataptr());
            thr.out_img[thr.i0 - thr.b] = m_out_img - thr.j0;
            for (int i = thr.i0 - thr.b + 1; i <= thr.i1 + thr.b; i++)
                thr.out_img[i] = thr.out_img[i - 1] + ((thr.j1 - thr.j0) + 1 + 2 * thr.b);

            threshold(thr.in_img, thr.out_img, thr.i0, thr.i1, thr.j0, thr.j1, thr.thr_val);
            return aff3ct::module::status_t::SUCCESS;
        });
    }

    virtual ~Threshold() {
        free(this->in_img + this->i0 - this->b);
        free(this->out_img + this->i0 - this->b);
    }

    virtual Threshold* clone() const {
        auto m = new Threshold(*this);
        m->deep_copy(*this);
        return m;
    }

    void deep_copy(const Threshold &m)
    {
        Module::deep_copy(m);
        this->init_data();
    }

    inline uint8_t** get_out_img() {
        return this->out_img;
    }

    inline aff3ct::module::Task& operator[](const thr::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const thr::sck::apply s) {
        return aff3ct::module::Module::operator[]((size_t)thr::tsk::apply)[(size_t)s];
    }

protected:
    void init_data() {
        this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
        this->out_img = (uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint8_t*)));
        this->in_img -= i0 - b;
        this->out_img -= i0 - b;
    }
};
