/*!
 * \file
 * \brief C++ wrapper for the CCL algorithms.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

#include "fmdt/CCL.h"

namespace ccl {
    enum class tsk : size_t { apply, SIZE };
    namespace sck {
        enum class apply : size_t { in_img, out_labels, out_n_RoIs, status };
    }
}

class CCL : public spu::module::Stateful {
protected:
    const int i0, i1, j0, j1;
    const int b;
    CCL_gen_data_t *data;

public:
    CCL(const int i0, const int i1, const int j0, const int j1, const int b, const enum ccl_impl_e impl = LSLH);
    virtual ~CCL();
    virtual CCL* clone() const;
    inline uint32_t** get_out_labels();
    inline spu::runtime::Task& operator[](const ccl::tsk t);
    inline spu::runtime::Socket& operator[](const ccl::sck::apply s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);

protected:
    void init_data(const enum ccl_impl_e impl);
    using spu::module::Stateful::deep_copy;
    void deep_copy(const CCL &m);
};

#include "fmdt/spu/CCL/CCL.hxx"
