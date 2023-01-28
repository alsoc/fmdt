#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ftr_mgn {
    enum class tsk : size_t { compute, SIZE };
    namespace sck {
        enum class compute : size_t { in_img, in_labels, in_ROI_xmin, in_ROI_xmax, in_ROI_ymin, in_ROI_ymax, in_ROI_S,
                                      in_n_ROI, out_ROI_magnitude, status };
    }
}

class Features_magnitude : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const size_t max_ROI_size;
    const uint8_t** in_img;
    const uint32_t** in_labels;
public:
    Features_magnitude(const int i0, const int i1, const int j0, const int j1, const int b, const size_t max_ROI_size);
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
