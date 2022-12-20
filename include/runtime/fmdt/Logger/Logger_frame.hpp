#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace lgr_fra {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
#ifdef OPENCV_LINK
        enum class write : size_t { in_labels, in_frame, in_ROI_id, in_ROI_xmax, in_ROI_ymin, in_ROI_ymax, in_n_ROI,
                                    status };
#else
        enum class write : size_t { in_labels, in_frame, status };
#endif
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
#ifdef OPENCV_LINK
    const int show_id;
#endif
    const uint32_t** in_labels;
    uint8_t** img_tmp;
public:
#ifdef OPENCV_LINK
    Logger_frame(const std::string frames_path, const int i0, const int i1, const int j0, const int j1, const int b,
                 const int show_id, const size_t max_ROI_size);
#else
    Logger_frame(const std::string frames_path, const int i0, const int i1, const int j0, const int j1, const int b);
#endif
    virtual ~Logger_frame();
    inline aff3ct::runtime::Task& operator[](const lgr_fra::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_fra::sck::write s);
};

#include "fmdt/Logger/Logger_frame.hxx"
