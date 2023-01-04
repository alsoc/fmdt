#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/tools.h"
#include "fmdt/video.h"

namespace lgr_fra {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_labels, in_ROI_id, in_ROI_xmax, in_ROI_ymin, in_ROI_ymax, in_n_ROI, status };
    }
}

class Logger_frame : public aff3ct::module::Module {
protected:
    const std::string frames_path;
    const int i0;
    const int i1;
    const int j0;
    const int j1;
    const int b;
    const uint8_t show_id;
    const uint32_t** in_labels;
    img_data_t* img_data;
    video_writer_t* video_writer;
public:
    Logger_frame(const std::string frames_path, const size_t fra_start, const int show_id, const int i0, const int i1,
                 const int j0, const int j1, const int b, const size_t max_ROI_size);
    virtual ~Logger_frame();
    inline aff3ct::runtime::Task& operator[](const lgr_fra::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_fra::sck::write s);
};

#include "fmdt/Logger/Logger_frame.hxx"
