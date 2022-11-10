#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace ftr_ext {
    enum class tsk : size_t { extract, SIZE };
    namespace sck {
        enum class extract : size_t { in_img, in_n_ROI, out_ROI_id, out_ROI_xmin, out_ROI_xmax, out_ROI_ymin,
                                      out_ROI_ymax, out_ROI_S, out_ROI_Sx, out_ROI_Sy, out_ROI_x, out_ROI_y, status };
    }
}

class Features_extractor : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const size_t max_ROI_size;
    const uint32_t** in_img;
public:
    Features_extractor(const int i0, const int i1, const int j0, const int j1, const int b, const size_t max_ROI_size);
    virtual ~Features_extractor();
    virtual Features_extractor* clone() const;
    inline aff3ct::runtime::Task& operator[](const ftr_ext::tsk t);
    inline aff3ct::runtime::Socket& operator[](const ftr_ext::sck::extract s);
protected:
    void init_data();
    using Module::deep_copy;
    void deep_copy(const Features_extractor &m);
};

#include "fmdt/Features/Features_extractor.hxx"