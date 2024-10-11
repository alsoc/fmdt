/*!
 * \file
 * \brief C++ wrapper for visualization.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

#include "fmdt/visu/visu_struct.h"

namespace vis {
    enum class tsk : size_t { display, SIZE };
    namespace sck {
        enum class display : size_t { in_frame, in_img, in_RoIs_basic, in_n_RoIs, status };
    }
}

class Visu : public spu::module::Stateful {
protected:
    const int i0;
    const int i1;
    const int j0;
    const int j1;
    const int b;
    const tracking_data_t* tracking_data;
    visu_data_t* visu_data;
public:
    Visu(const char* path, const size_t start, const size_t n_ffmpeg_threads, const int i0, const int i1,
         const int j0, const int j1, const int b, const enum pixfmt_e pixfmt, const enum video_codec_e codec_type,
         const uint8_t draw_track_id, const uint8_t draw_legend, const int win_play, const size_t buff_size,
         const size_t max_RoIs_size, const uint8_t skip_fra, const tracking_data_t* tracking_data);
    virtual ~Visu();
    void flush();
    inline spu::runtime::Task& operator[](const vis::tsk t);
    inline spu::runtime::Socket& operator[](const vis::sck::display s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Visu/Visu.hxx"
