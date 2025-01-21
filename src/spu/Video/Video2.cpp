#include "fmdt/video/video_io.h"
#include "fmdt/image/image_compute.h"
#include "fmdt/tools.h"

#include "fmdt/spu/Video/Video2.hpp"

Video2::Video2(const std::string filename, const size_t frame_start, const size_t frame_end, const size_t frame_skip,
               const int bufferize, const size_t n_ffmpeg_threads, const int b, const enum video_codec_e codec_type,
               const enum video_codec_hwaccel_e hwaccel, const enum pixfmt_e pixfmt, const bool ffmpeg_debug,
               const char* ffmpeg_in_extra_opts)
: spu::module::Stateful(), i0(0), i1(0), j0(0), j1(0), b(b), video(nullptr), img_buf(nullptr), done(false) {
    const std::string name = "Video2";
    this->set_name(name);
    this->set_short_name(name);

    this->video = video_reader_alloc_init(filename.c_str(), frame_start, frame_end, frame_skip, bufferize,
                                          n_ffmpeg_threads, codec_type, hwaccel, pixfmt, ffmpeg_debug,
                                          ffmpeg_in_extra_opts, &this->i0, &this->i1, &this->j0, &this->j1);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;
    const size_t pixsize = image_get_pixsize(pixfmt);

    this->size_image = img_n_rows * img_n_cols * pixsize;
    this->out_pixfmt = pixfmt;

    this->img_buf = (uint8_t*)malloc((size_t)(this->size_image * sizeof(uint8_t)));
    std::fill(this->img_buf, this->img_buf + this->size_image, 0);

    auto &p = this->create_task("generate");
    auto ps_out_img0  = this->template create_2d_socket_out<uint8_t>(p, "out_img0", img_n_rows, img_n_cols * pixsize);
    auto ps_out_img1  = this->template create_2d_socket_out<uint8_t>(p, "out_img1", img_n_rows, img_n_cols * pixsize);
    auto ps_out_frame = this->template create_socket_out<uint32_t>(p, "out_frame", 1);

    this->create_codelet(p, [ps_out_img0, ps_out_img1, ps_out_frame]
                            (spu::module::Module &m, spu::runtime::Task &t,const size_t frame_id) -> int {
        auto &vid2 = static_cast<Video2&>(m);

        uint8_t* out_1d_img0 = static_cast<uint8_t*>(t[ps_out_img0].get_dataptr());
        memcpy(out_1d_img0, vid2.img_buf, vid2.size_image);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        uint8_t** out_img1 = t[ps_out_img1].get_2d_dataptr<uint8_t>(vid2.b, vid2.b);

        int cur_fra = video_reader_get_frame(vid2.video, out_img1, NULL);
        vid2.done = cur_fra == -1 ? true : false;

        uint8_t* out_1d_img1 = static_cast<uint8_t*>(t[ps_out_img1].get_dataptr());
        memcpy(vid2.img_buf, out_1d_img1, vid2.size_image);

        if (vid2.done)
            throw spu::tools::processing_aborted(__FILE__, __LINE__, __func__);

        *static_cast<uint32_t*>(t[ps_out_frame].get_dataptr()) = (uint32_t)cur_fra;

        return spu::runtime::status_t::SUCCESS;
    });

    auto &convert = this->create_task("gray2rgb");
    auto converts_fwd_img_gray8  = this->template create_2d_socket_in<uint8_t>(convert, "fwd_img_gray8", img_n_rows, img_n_cols);
    auto converts_out_img_rgb24 = this->template create_2d_socket_out<uint8_t>(convert, "out_img_rgb24", img_n_rows, img_n_cols * 3);
    this->create_codelet(convert, [converts_fwd_img_gray8, converts_out_img_rgb24]
                            (spu::module::Module &m, spu::runtime::Task &t,const size_t frame_id) -> int {
        auto &vid = static_cast<Video2&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint8_t** fwd_img_gray8  = t[converts_fwd_img_gray8].get_2d_dataptr<const uint8_t>(vid.b, vid.b);
        uint8_t** out_img_rgb24        = t[converts_out_img_rgb24].get_2d_dataptr<uint8_t>(vid.b, vid.b);

        image_convert_gray8_to_rgb24(fwd_img_gray8, vid.i0, vid.i1, vid.j0, vid.j1, out_img_rgb24);

        return spu::runtime::status_t::SUCCESS;
    });
}

Video2::~Video2() {
    free(this->img_buf);
    video_reader_free(this->video);
}

bool Video2::is_done() const {
    return this->done;
}
