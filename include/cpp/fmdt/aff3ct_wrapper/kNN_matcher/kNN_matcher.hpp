/*!
 * \file
 * \brief C++ wrapper to compute the k-NN matching algorithm.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace knn {
    enum class tsk : size_t { match, SIZE };
    namespace sck {
        enum class match : size_t { in_RoIs0_id, in_RoIs0_S, in_RoIs0_x, in_RoIs0_y, in_n_RoIs0, in_RoIs1_id,
                                    in_RoIs1_S, in_RoIs1_x, in_RoIs1_y, in_n_RoIs1, out_RoIs0_next_id,
                                    out_RoIs1_prev_id, out_data_nearest, out_data_distances, out_data_conflicts,
                                    status };
    }
}
#endif

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "fmdt/aff3ct_wrapper/kNN_matcher/kNN_matcher.hxx"
#endif
