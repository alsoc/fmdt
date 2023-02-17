/*!
 * \file
 * \brief C++ wrapper for the CCL LSL algorithm.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/CCL.h"

namespace ccl {
    enum class tsk : size_t { apply, SIZE };
    namespace sck {
        enum class apply : size_t { in_img, out_labels, out_n_RoIs, status };
    }
}

class CCL_LSL : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    CCL_gen_data_t *data;
    const uint8_t** in_img;
    uint32_t** out_labels;

public:
    CCL_LSL(const int i0, const int i1, const int j0, const int j1, const int b, const enum ccl_impl_e impl = LSLH);
    virtual ~CCL_LSL();
    virtual CCL_LSL* clone() const;
    inline uint32_t** get_out_labels();
    inline aff3ct::runtime::Task& operator[](const ccl::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ccl::sck::apply s);

protected:
    void init_data(const enum ccl_impl_e impl);
    using Module::deep_copy;
    void deep_copy(const CCL_LSL &m);
};

#include "fmdt/aff3ct_wrapper/CCL_LSL/CCL_LSL.hxx"
