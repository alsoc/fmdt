#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace vid {
    enum class tsk : size_t { generate, SIZE };
    namespace sck {
        enum class generate : size_t { out_img, out_frame, status };
    }
}

class Video : public aff3ct::module::Module, public aff3ct::tools::Interface_is_done {
protected:
    int i0, i1, j0, j1;
    int b;
    video_t* video;
    uint8_t** out_img;
    bool done;
public:
    Video(const std::string filename, const size_t frame_start, const size_t frame_end, const size_t frame_skip,
          const size_t n_ffmpeg_threads, const int b);
    virtual ~Video();
    virtual bool is_done() const;
    inline uint8_t** get_out_img();
    inline int get_i0();
    inline int get_i1();
    inline int get_j0();
    inline int get_j1();
    inline int get_b();
    inline aff3ct::runtime::Task& operator[](const vid::tsk t);
    inline aff3ct::runtime::Socket& operator[](const vid::sck::generate s);
};

#include "fmdt/Video/Video.hxx"
