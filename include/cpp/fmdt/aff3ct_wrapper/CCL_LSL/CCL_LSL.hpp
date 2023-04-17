/*!
 * \file
 * \brief C++ wrapper for the CCL LSL algorithm.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ccl {
    enum class tsk : size_t { apply, SIZE };
    namespace sck {
        enum class apply : size_t { in_img, out_labels, out_n_RoIs, out_data_er, out_data_era, out_data_rlc,
                                    out_data_eq, out_data_ner, status };
    }
}

class CCL_LSL : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    // CCL_data_t *data;
    const uint8_t** in_img;
    uint32_t** out_labels;
    uint32_t** out_data_er;
    uint32_t** out_data_era;
    uint32_t** out_data_rlc;
public:
    CCL_LSL(const int i0, const int i1, const int j0, const int j1, const int b);
    virtual ~CCL_LSL();
    virtual CCL_LSL* clone() const;
    inline uint32_t** get_out_labels();
    inline aff3ct::runtime::Task& operator[](const ccl::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ccl::sck::apply s);

protected:
    void init_data();
    using Module::deep_copy;
    void deep_copy(const CCL_LSL &m);
};

#include "fmdt/aff3ct_wrapper/CCL_LSL/CCL_LSL.hxx"
