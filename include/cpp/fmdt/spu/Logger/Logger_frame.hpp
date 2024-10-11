/*!
 * \file
 * \brief C++ wrapper to log 2D array of labels (after CCL).
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

#include "fmdt/image/image_struct.h"
#include "fmdt/video/video_struct.h"

namespace lgr_fra {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_labels, in_RoIs_basic, in_n_RoIs, status };
    }
}

class Logger_frame : public spu::module::Stateful {
protected:
    const std::string frames_path;
    const int i0;
    const int i1;
    const int j0;
    const int j1;
    const int b;
    const uint8_t show_id;
    img_data_t* img_data;
    video_writer_t* video_writer;
public:
    Logger_frame(const std::string frames_path, const size_t fra_start, const int show_id, const int i0, const int i1,
                 const int j0, const int j1, const int b, const size_t max_RoIs_size);
    virtual ~Logger_frame();
    inline spu::runtime::Task& operator[](const lgr_fra::tsk t);
    inline spu::runtime::Socket& operator[](const lgr_fra::sck::write s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Logger/Logger_frame.hxx"
