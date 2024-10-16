#include "fmdt/visu/visu_io.h"
#include "fmdt/tools.h"

#include "fmdt/spu/Visu/Visu.hpp"

Visu::Visu(const char* path, const size_t start, const size_t n_ffmpeg_threads, const int i0, const int i1,
           const int j0, const int j1, const int b, const enum pixfmt_e pixfmt_in, const enum pixfmt_e pixfmt_out,
           const enum video_codec_e codec_type, const uint8_t draw_track_id, const uint8_t draw_legend,
           const int win_play, const bool ffmpeg_debug, const char* ffmpeg_out_extra_opts, const size_t buff_size,
           const size_t max_RoIs_size, const uint8_t skip_fra, const tracking_data_t* tracking_data)
: spu::module::Stateful(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), tracking_data(tracking_data), visu_data(nullptr) {
    assert(tracking_data != NULL);

    const std::string name = "Visu";
    this->set_name(name);
    this->set_short_name(name);

    this->visu_data = visu_alloc_init(path, start, n_ffmpeg_threads, (i1 - i0) + 1, (j1 - j0) + 1, pixfmt_in,
                                      pixfmt_out, codec_type, draw_track_id, draw_legend, win_play, ffmpeg_debug,
                                      ffmpeg_out_extra_opts, buff_size, max_RoIs_size, skip_fra);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("display");
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);
    size_t pixsize = image_get_pixsize(pixfmt_in);
    auto ps_in_img = this->template create_2d_socket_in<uint8_t>(p, "in_img", img_n_rows, img_n_cols * pixsize);
    auto ps_in_RoIs_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs_basic",
                                                                     max_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);

    this->create_codelet(p, [ps_in_frame, ps_in_img, ps_in_RoIs_basic, ps_in_n_RoIs]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &vis = static_cast<Visu&>(m);

        const uint32_t     in_frame      = *t[ps_in_frame     ].get_dataptr   <const uint32_t   >();
        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint8_t**    in_img        =  t[ps_in_img       ].get_2d_dataptr<const uint8_t    >(vis.b, vis.b);
        const RoI_basic_t* in_RoIs_basic =  t[ps_in_RoIs_basic].get_dataptr   <const RoI_basic_t>();
        const uint32_t     in_n_RoIs     = *t[ps_in_n_RoIs    ].get_dataptr   <const uint32_t   >();

        visu_display(vis.visu_data, in_img, in_RoIs_basic, in_n_RoIs, vis.tracking_data->tracks, in_frame);

        return spu::runtime::status_t::SUCCESS;
    });
}

Visu::Visu(const char* path, const size_t start, const size_t n_ffmpeg_threads, const int i0, const int i1,
           const int j0, const int j1, const int b, const enum pixfmt_e pixfmt_in, const enum pixfmt_e pixfmt_out,
           const enum video_codec_e codec_type, const uint8_t draw_track_id, const uint8_t draw_legend,
           const int win_play, const size_t buff_size, const size_t max_RoIs_size, const uint8_t skip_fra,
           const tracking_data_t* tracking_data)
: Visu(path, start, n_ffmpeg_threads, i0, i1, j0, j1, b, pixfmt_in, pixfmt_out, codec_type, draw_track_id, draw_legend,
       win_play, false, nullptr, buff_size, max_RoIs_size, skip_fra, tracking_data)
{
}

Visu::~Visu() {
    visu_free(this->visu_data);
}

void Visu::flush() {
    visu_flush(this->visu_data, this->tracking_data->tracks);
}
