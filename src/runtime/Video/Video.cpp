#include "fmdt/video.h"
#include "fmdt/tools.h"

#include "fmdt/Video/Video.hpp"

Video::Video(const std::string filename, const size_t frame_start, const size_t frame_end, const size_t frame_skip,
             const size_t n_ffmpeg_threads, const int b)
: Module(), i0(0), i1(0), j0(0), j1(0), b(b), video(nullptr), out_img(nullptr), done(false) {
    const std::string name = "Video";
    this->set_name(name);
    this->set_short_name(name);

    this->video = video_init_from_file(filename.c_str(), frame_start, frame_end, frame_skip, n_ffmpeg_threads,
                                       &this->i0, &this->i1, &this->j0, &this->j1);

    this->out_img = (uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint8_t*)));
    this->out_img -= i0 - b;

    auto socket_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("generate");
    auto ps_out_img = this->template create_socket_out<uint8_t>(p, "out_img", socket_size);
    auto ps_out_frame = this->template create_socket_out<uint32_t>(p, "out_frame", 1);

    this->create_codelet(p, [ps_out_img, ps_out_frame]
                            (aff3ct::module::Module &m, aff3ct::runtime::Task &t,const size_t frame_id) -> int {
        auto &vid = static_cast<Video&>(m);
        
        uint8_t* m_out_img = static_cast<uint8_t*>(t[ps_out_img].get_dataptr());
        tools_linear_2d_nrc_ui8matrix((const uint8_t*)m_out_img, vid.i0 - vid.b, vid.i1 + vid.b, vid.j0 - vid.b, 
                                      vid.j1 + vid.b, (const uint8_t**)vid.out_img);
 
        *static_cast<uint32_t*>(t[ps_out_frame].get_dataptr()) = vid.video->frame_current;
        int ret = video_get_next_frame(vid.video, vid.out_img);
        vid.done = ret ? false : true;
        if (vid.done)
            throw aff3ct::tools::processing_aborted(__FILE__, __LINE__, __func__);
        return ret ? aff3ct::runtime::status_t::SUCCESS : aff3ct::runtime::status_t::FAILURE_STOP;
    });
}

Video::~Video() {
    free(this->out_img + this->i0 - this->b);
    video_free(this->video);
}

bool Video::is_done() const {
    return this->done;
}
