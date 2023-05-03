/*!
 * \file
 * \brief C++ wrapper to compute ellipse features.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ftr_ell {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_RoIs_S, in_RoIs_Sx, in_RoIs_Sy, in_RoIs_Sx2, in_RoIs_Sy2, in_RoIs_Sxy,
                                      in_n_RoIs, out_RoIs_a, out_RoIs_b, status };
    }
}

class Features_ellipse : public aff3ct::module::Module {
protected:
    const size_t max_RoIs_size;
public:
    Features_ellipse(const size_t max_RoIs_size);
    virtual ~Features_ellipse() = default;
    virtual Features_ellipse* clone() const;
    inline aff3ct::runtime::Task& operator[](const ftr_ell::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ftr_ell::sck::compute s);
};

#include "fmdt/aff3ct_wrapper/Features/Features_ellipse.hxx"
