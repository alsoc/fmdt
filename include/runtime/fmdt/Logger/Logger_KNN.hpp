#pragma once

#include <stdint.h>
#include <aff3ct_core.hpp>

namespace lgr_knn {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_data_nearest, in_data_distances, in_ROI_id, in_ROI_S, in_ROI_dx, in_ROI_dy,
                                    in_ROI_error, in_ROI_next_id, in_n_ROI, in_frame, status };
    }
}

class Logger_KNN : public aff3ct::module::Module {
protected:
    const std::string KNN_path;
    const int i0;
    const int i1;
    const int j0;
    const int j1;
    const uint32_t** in_data_nearest;
    const float** in_data_distances;

public:
    Logger_KNN(const std::string KNN_path, const size_t i0, const int i1, const int j0, const int j1,
               const size_t max_ROI_size);
    virtual ~Logger_KNN();
    inline aff3ct::module::Task& operator[](const lgr_knn::tsk t);
    inline aff3ct::module::Socket& operator[](const lgr_knn::sck::write s);
};

#include "fmdt/Logger/Logger_KNN.hxx"
