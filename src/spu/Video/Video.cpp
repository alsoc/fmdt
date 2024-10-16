#include "fmdt/video/video_io.h"
#include "fmdt/tools.h"

#include "fmdt/spu/Video/Video.hpp"

Video::Video(const std::string filename, const size_t frame_start, const size_t frame_end, const size_t frame_skip,
             const int bufferize, const size_t n_ffmpeg_threads, const int b, const enum video_codec_e codec_type,
             const enum video_codec_hwaccel_e hwaccel, const enum pixfmt_e pixfmt, const bool ffmpeg_debug,
             const char* ffmpeg_in_extra_opts)
: spu::module::Stateful(), i0(0), i1(0), j0(0), j1(0), b(b), video(nullptr), done(false) {
    const std::string name = "Video";
    this->set_name(name);
    this->set_short_name(name);

    this->video = video_reader_alloc_init(filename.c_str(), frame_start, frame_end, frame_skip, bufferize,
                                          n_ffmpeg_threads, codec_type, hwaccel, pixfmt, ffmpeg_debug,
                                          ffmpeg_in_extra_opts, &this->i0, &this->i1, &this->j0, &this->j1);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("generate");
    auto ps_out_img_gray8 = this->template create_2d_socket_out<uint8_t>(p, "out_img_gray8", img_n_rows, img_n_cols);
    size_t ps_out_img_rgb24 = 0;
    if (pixfmt == PIXFMT_RGB24)
        ps_out_img_rgb24 = this->template create_2d_socket_out<uint8_t>(p, "out_img_rgb24", img_n_rows, img_n_cols * 3);
    auto ps_out_frame = this->template create_socket_out<uint32_t>(p, "out_frame", 1);

    this->create_codelet(p, [ps_out_img_gray8, ps_out_img_rgb24, ps_out_frame]
                            (spu::module::Module &m, spu::runtime::Task &t,const size_t frame_id) -> int {
        auto &vid = static_cast<Video&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        uint8_t** out_img_gray8 = t[ps_out_img_gray8].get_2d_dataptr<uint8_t>(vid.b, vid.b);
        uint8_t** out_img_rgb24 = (ps_out_img_rgb24) ? t[ps_out_img_rgb24].get_2d_dataptr<uint8_t>() : nullptr;

        int cur_fra = video_reader_get_frame(vid.video, out_img_gray8, out_img_rgb24);
        vid.done = cur_fra == -1 ? true : false;
        if (vid.done)
            throw spu::tools::processing_aborted(__FILE__, __LINE__, __func__);

        *static_cast<uint32_t*>(t[ps_out_frame].get_dataptr()) = (uint32_t)cur_fra;

        return spu::runtime::status_t::SUCCESS;
    });
}

Video::~Video() {
    video_reader_free(this->video);
}

bool Video::is_done() const {
    return this->done;
}
