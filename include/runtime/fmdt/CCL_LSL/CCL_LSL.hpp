#pragma once

#include <stdint.h>
#include <aff3ct_core.hpp>

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
    CCL_LSL(const int i0, const int i1, const int j0, const int j1, const int b);
    virtual ~CCL_LSL();
    virtual CCL_LSL* clone() const;
    inline uint32_t** get_out_img();
    inline aff3ct::module::Task& operator[](const ccl::tsk t);
    inline aff3ct::module::Socket& operator[](const ccl::sck::apply s);

protected:
    void init_data();
    void deep_copy(const CCL_LSL &m);
};

#include "fmdt/CCL_LSL/CCL_LSL.hxx"