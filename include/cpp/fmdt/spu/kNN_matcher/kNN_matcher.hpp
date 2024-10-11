/*!
 * \file
 * \brief C++ wrapper to compute the k-NN matching algorithm.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

namespace knn {
    enum class tsk : size_t { match, SIZE };
    namespace sck {
        enum class match : size_t { in_RoIs0_basic, in_n_RoIs0, in_RoIs1_basic, in_n_RoIs1, out_RoIs0_asso,
                                    out_RoIs1_asso, out_data_distances, out_data_nearest, out_data_conflicts,
                                    status };
    }
}

class kNN_matcher : public spu::module::Stateful {
protected:
    const size_t k;
    const uint32_t max_dist;
    const float min_ratio_S;
    const size_t max_size;
public:
    kNN_matcher(const size_t k, const uint32_t max_dist, const float min_ratio_S, const size_t max_size);
    virtual ~kNN_matcher();
    virtual kNN_matcher* clone() const;
    inline spu::runtime::Task& operator[](const knn::tsk t);
    inline spu::runtime::Socket& operator[](const knn::sck::match s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/kNN_matcher/kNN_matcher.hxx"
