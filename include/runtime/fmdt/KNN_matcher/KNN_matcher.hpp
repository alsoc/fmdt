#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace knn {
    enum class tsk : size_t { match, SIZE };
    namespace sck {
        enum class match : size_t { in_ROI0_id, in_ROI0_S, in_ROI0_x, in_ROI0_y, in_n_ROI0, in_ROI1_id, in_ROI1_S,
                                    in_ROI1_x, in_ROI1_y, in_n_ROI1, out_ROI0_next_id, out_ROI1_prev_id,
                                    out_data_nearest, out_data_distances, out_data_conflicts, status };
    }
}

class KNN_matcher : public aff3ct::module::Module {
protected:
    const size_t k;
    const uint32_t max_dist;
    const float min_ratio_S;
    const size_t max_size;
    uint32_t** out_data_nearest;
    float** out_data_distances;
public:
    KNN_matcher(const size_t k, const uint32_t max_dist, const float min_ratio_S, const size_t max_size);
    virtual ~KNN_matcher();
    inline aff3ct::runtime::Task& operator[](const knn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const knn::sck::match s);
};

#include "fmdt/KNN_matcher/KNN_matcher.hxx"
