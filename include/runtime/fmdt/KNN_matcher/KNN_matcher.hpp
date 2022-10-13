#pragma once

#include <stdint.h>
#include <aff3ct_core.hpp>

namespace knn {
    enum class tsk : size_t { match, SIZE };
    namespace sck {
        enum class match : size_t { in_ROI0_id, in_ROI0_x, in_ROI0_y, in_n_ROI0, in_ROI1_id, in_ROI1_x, in_ROI1_y,
                                    in_n_ROI1, out_ROI0_next_id, out_ROI1_prev_id, out_data_nearest, out_data_distances,
                                    out_data_conflicts, status };
    }
}

class KNN_matcher : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const size_t k;
    uint32_t** out_data_nearest;
    float** out_data_distances;
public:
    KNN_matcher(const size_t i0, const int i1, const int j0, const int j1, const size_t k, const size_t max_ROI_size);
    virtual ~KNN_matcher();
    inline aff3ct::module::Task& operator[](const knn::tsk t);
    inline aff3ct::module::Socket& operator[](const knn::sck::match s);
};

#include "fmdt/KNN_matcher/KNN_matcher.hxx"
