/*!
 * \file
 * \brief C++ wrapper for labels zero init.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ftr_lzi {
    enum class tsk : size_t { zinit, SIZE };
    namespace sck {
        enum class zinit : size_t { fwd_labels, in_RoIs_xmin, in_RoIs_xmax, in_RoIs_ymin, in_RoIs_ymax, in_n_RoIs,
                                    status };
    }
}

class Features_labels_zero_init : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const size_t max_in_RoIs_size;
public:
    Features_labels_zero_init(const int i0, const int i1, const int j0, const int j1, const int b,
                              const size_t max_in_RoIs_size);
    virtual ~Features_labels_zero_init();
    virtual Features_labels_zero_init* clone() const;

    inline aff3ct::runtime::Task& operator[](const ftr_lzi::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ftr_lzi::sck::zinit s);
    inline aff3ct::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/aff3ct_wrapper/Features/Features_labels_zero_init.hxx"
