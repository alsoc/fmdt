#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/image/image_struct.h"
#include "fmdt/video/video_struct.h"

namespace lgr_fra {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_labels, in_RoI_id, in_RoI_xmax, in_RoI_ymin, in_RoI_ymax, in_n_RoI, status };
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
                 const int j0, const int j1, const int b, const size_t max_RoI_size);
    virtual ~Logger_frame();
    inline aff3ct::runtime::Task& operator[](const lgr_fra::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_fra::sck::write s);
};

#include "fmdt/aff3ct_wrapper/Logger/Logger_frame.hxx"
