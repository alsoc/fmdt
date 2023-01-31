#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tracking/tracking_struct.h"

namespace lgr_roi {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_RoI0_id, in_RoI0_xmin, in_RoI0_xmax, in_RoI0_ymin, in_RoI0_ymax,
                                    in_RoI0_S, in_RoI0_Sx, in_RoI0_Sy, in_RoI0_x, in_RoI0_y, in_RoI0_magnitude,
                                    in_n_RoI0, in_RoI1_id, in_RoI1_xmin, in_RoI1_xmax, in_RoI1_ymin, in_RoI1_ymax,
                                    in_RoI1_S, in_RoI1_Sx, in_RoI1_Sy, in_RoI1_x, in_RoI1_y, in_RoI1_magnitude,
                                    in_n_RoI1, in_frame, status};
    }
}

class Logger_RoI : public aff3ct::module::Module {
protected:
    const std::string RoI_path;
    const size_t fra_start;
    const size_t fra_skip;
    const tracking_data_t* tracking_data;
public:
    Logger_RoI(const std::string RoI_path, const size_t fra_start, const size_t frame_skip, const size_t max_RoI_size,
               const tracking_data_t* tracking_data);
    virtual ~Logger_RoI();
    inline aff3ct::runtime::Task& operator[](const lgr_roi::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_roi::sck::write s);
};

#include "fmdt/aff3ct_wrapper/Logger/Logger_RoI.hxx"
