#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ftr_ext {
    enum class tsk : size_t { extract, SIZE };
    namespace sck {
        enum class extract : size_t { in_img, in_n_RoI, out_RoI_id, out_RoI_xmin, out_RoI_xmax, out_RoI_ymin,
                                      out_RoI_ymax, out_RoI_S, out_RoI_Sx, out_RoI_Sy, out_RoI_x, out_RoI_y, status };
    }
}

class Features_extractor : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const size_t max_RoI_size;
    const uint32_t** in_img;
public:
    Features_extractor(const int i0, const int i1, const int j0, const int j1, const int b, const size_t max_RoI_size);
    virtual ~Features_extractor();
    virtual Features_extractor* clone() const;
    inline aff3ct::runtime::Task& operator[](const ftr_ext::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ftr_ext::sck::extract s);
protected:
    void init_data();
    using Module::deep_copy;
    void deep_copy(const Features_extractor &m);
};

#include "fmdt/aff3ct_wrapper/Features/Features_extractor.hxx"