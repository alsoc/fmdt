#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace knn {
    enum class tsk : size_t { match, SIZE };
    namespace sck {
        enum class match : size_t { in_RoI0_id, in_RoI0_S, in_RoI0_x, in_RoI0_y, in_n_RoI0, in_RoI1_id, in_RoI1_S,
                                    in_RoI1_x, in_RoI1_y, in_n_RoI1, out_RoI0_next_id, out_RoI1_prev_id,
                                    out_data_nearest, out_data_distances, out_data_conflicts, status };
    }
}

class kNN_matcher : public aff3ct::module::Module {
protected:
    const size_t k;
    const uint32_t max_dist;
    const float min_ratio_S;
    const size_t max_size;
    uint32_t** out_data_nearest;
    float** out_data_distances;
public:
    kNN_matcher(const size_t k, const uint32_t max_dist, const float min_ratio_S, const size_t max_size);
    virtual ~kNN_matcher();
    inline aff3ct::runtime::Task& operator[](const knn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const knn::sck::match s);
};

#include "fmdt/aff3ct_wrapper/kNN_matcher/kNN_matcher.hxx"
