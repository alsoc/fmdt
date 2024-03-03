/*!
 * \file
 * \brief C++ wrapper to log kNN statistics.
 */

#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace lgr_knn {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_data_nearest, in_data_distances, in_data_conflicts, in_RoIs0_basic,
                                    in_RoIs0_asso, in_n_RoIs0, in_RoIs1_error, in_n_RoIs1, in_frame, status };
    }
}

class Logger_kNN : public aff3ct::module::Module {
protected:
    const std::string kNN_path;
    const size_t fra_start;
    const size_t max_size;

public:
    Logger_kNN(const std::string kNN_path, const size_t fra_start, const size_t max_size);
    virtual ~Logger_kNN();
    inline aff3ct::runtime::Task& operator[](const lgr_knn::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_knn::sck::write s);
    inline aff3ct::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/aff3ct_wrapper/Logger/Logger_kNN.hxx"
