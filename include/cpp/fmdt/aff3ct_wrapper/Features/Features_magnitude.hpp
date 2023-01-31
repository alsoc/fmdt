#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ftr_mgn {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_img, in_labels, in_RoI_xmin, in_RoI_xmax, in_RoI_ymin, in_RoI_ymax, in_RoI_S,
                                      in_n_RoI, out_RoI_magnitude, status };
    }
}

class Features_magnitude : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const size_t max_RoI_size;
    const uint8_t** in_img;
    const uint32_t** in_labels;
public:
    Features_magnitude(const int i0, const int i1, const int j0, const int j1, const int b, const size_t max_RoI_size);
    virtual ~Features_magnitude();
    virtual Features_magnitude* clone() const;
    inline aff3ct::runtime::Task& operator[](const ftr_mgn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ftr_mgn::sck::compute s);
protected:
    void init_data();
    using Module::deep_copy;
    void deep_copy(const Features_magnitude &m);
};

#include "fmdt/aff3ct_wrapper/Features/Features_magnitude.hxx"
