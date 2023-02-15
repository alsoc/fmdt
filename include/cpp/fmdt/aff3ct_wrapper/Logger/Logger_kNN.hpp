/*!
 * \file
 * \brief C++ wrapper to log kNN statistics.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace lgr_knn {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_data_nearest, in_data_distances, in_data_conflicts, in_RoIs0_id,
                                    in_RoIs0_next_id, in_n_RoIs0, in_RoIs1_dx, in_RoIs1_dy, in_RoIs1_error,
                                    in_RoIs1_is_moving, in_n_RoIs1, in_frame, status };
    }
}
#endif

class Logger_kNN : public aff3ct::module::Module {
protected:
    const std::string kNN_path;
    const size_t fra_start;
    const size_t max_size;
    const uint32_t** in_data_nearest;
    const float** in_data_distances;

public:
    Logger_kNN(const std::string kNN_path, const size_t fra_start, const size_t max_size);
    virtual ~Logger_kNN();
    inline aff3ct::runtime::Task& operator[](const lgr_knn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_knn::sck::write s);
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "fmdt/aff3ct_wrapper/Logger/Logger_kNN.hxx"
#endif
