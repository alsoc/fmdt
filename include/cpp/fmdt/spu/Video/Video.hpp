/*!
 * \file
 * \brief C++ wrapper to get image at \f$t\f$.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

#include "fmdt/video/video_struct.h"

namespace vid {
    enum class tsk : size_t { generate, SIZE };
    namespace sck {
        enum class generate : size_t { out_img, out_frame, status };
    }
}

class Video : public spu::module::Stateful, public spu::tools::Interface_is_done {
protected:
    int i0, i1, j0, j1;
    int b;
    video_reader_t* video;
    bool done;
public:
    Video(const std::string filename, const size_t frame_start, const size_t frame_end, const size_t frame_skip,
          const int bufferize, const size_t n_ffmpeg_threads, const int b,
          const enum video_codec_e codec_type = VCDC_FFMPEG_IO,
          const enum video_codec_hwaccel_e hwaccel = VCDC_HWACCEL_NONE, const enum pixfmt_e pixfmt = PIXFMT_GRAY8,
          const bool ffmpeg_debug = false, const char* ffmpeg_out_extra_opts = nullptr);
    virtual ~Video();
    virtual bool is_done() const;
    inline uint8_t** get_out_img();
    inline int get_i0();
    inline int get_i1();
    inline int get_j0();
    inline int get_j1();
    inline int get_b();
    inline void set_loop_size(size_t loop_size);
    inline spu::runtime::Task& operator[](const vid::tsk t);
    inline spu::runtime::Socket& operator[](const vid::sck::generate s);
    inline spu::runtime::Socket& operator[](const std::string &tsk_sck);
};

#include "fmdt/spu/Video/Video.hxx"
