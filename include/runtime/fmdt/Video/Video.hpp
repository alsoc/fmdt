#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

#include "fmdt/video.h"

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
          const int b)
    : Module(), i0(0), i1(0), j0(0), j1(0), b(b), video(nullptr), out_img(nullptr), done(false) {
        const std::string name = "Video";
        this->set_name(name);
        this->set_short_name(name);

        this->video = video_init_from_file(filename.c_str(), frame_start, frame_end, frame_skip, &this->i0, &this->i1,
                                           &this->j0, &this->j1);

        this->out_img = (uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint8_t*)));
        this->out_img -= i0 - b;

        auto socket_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

        auto &p = this->create_task("generate");
        auto ps_out_img = this->template create_socket_out<uint8_t>(p, "out_img", socket_size);
        auto ps_out_frame = this->template create_socket_out<uint32_t>(p, "out_frame", 1);

        this->create_codelet(p, [ps_out_img, ps_out_frame]
                                (aff3ct::module::Module &m, aff3ct::module::Task &t,const size_t frame_id) -> int {
            auto &vid = static_cast<Video&>(m);
            uint8_t* m_out_img = static_cast<uint8_t*>(t[ps_out_img].get_dataptr());
            vid.out_img[vid.i0 - vid.b] = m_out_img;
            for (int i = vid.i0 - vid.b + 1; i <= vid.i1 + vid.b; i++)
                vid.out_img[i] = vid.out_img[i - 1] + ((vid.j1 - vid.j0) + 1 + 2 * vid.b);

            *static_cast<uint32_t*>(t[ps_out_frame].get_dataptr()) = vid.video->frame_current;
            int ret = video_get_next_frame(vid.video, vid.out_img);
            vid.done = ret ? false : true;
            if (vid.done)
                throw aff3ct::tools::processing_aborted(__FILE__, __LINE__, __func__);
            return ret ? aff3ct::module::status_t::SUCCESS : aff3ct::module::status_t::FAILURE_STOP;
        });
    }

    virtual ~Video() {
        free(this->out_img + this->i0 - this->b);
        video_free(this->video);
    }

    inline uint8_t** get_out_img() {
        return this->out_img;
    }

    inline int get_i0() {
        return this->i0;
    }

    inline int get_i1() {
        return this->i1;
    }

    inline int get_j0() {
        return this->j0;
    }

    inline int get_j1() {
        return this->j1;
    }

    virtual bool is_done() const {
        return this->done;
    }

    inline aff3ct::module::Task& operator[](const vid::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const vid::sck::generate s) {
        return aff3ct::module::Module::operator[]((size_t)vid::tsk::generate)[(size_t)s];
    }
};
