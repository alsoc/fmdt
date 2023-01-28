#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tracking/tracking_struct.h"

namespace lgr_roi {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_ROI0_id, in_ROI0_xmin, in_ROI0_xmax, in_ROI0_ymin, in_ROI0_ymax,
                                    in_ROI0_S, in_ROI0_Sx, in_ROI0_Sy, in_ROI0_x, in_ROI0_y, in_ROI0_magnitude,
                                    in_n_ROI0, in_ROI1_id, in_ROI1_xmin, in_ROI1_xmax, in_ROI1_ymin, in_ROI1_ymax,
                                    in_ROI1_S, in_ROI1_Sx, in_ROI1_Sy, in_ROI1_x, in_ROI1_y, in_ROI1_magnitude,
                                    in_n_ROI1, in_frame, status};
    }
}

class Logger_ROI : public aff3ct::module::Module {
protected:
    const std::string ROI_path;
    const size_t fra_start;
    const size_t fra_skip;
    const tracking_data_t* tracking_data;
public:
    Logger_ROI(const std::string ROI_path, const size_t fra_start, const size_t frame_skip, const size_t max_ROI_size,
               const tracking_data_t* tracking_data);
    virtual ~Logger_ROI();
    inline aff3ct::runtime::Task& operator[](const lgr_roi::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_roi::sck::write s);
};

#include "fmdt/aff3ct_wrapper/Logger/Logger_ROI.hxx"
