#include <nrc2.h>

#include "fmdt/tools.h"
#include "fmdt/image/image_compute.h"
#include "fmdt/video/video_io.h"

#include "fmdt/spu/Logger/Logger_frame.hpp"

Logger_frame::Logger_frame(const std::string frames_path, const size_t fra_start, const int show_id, const int i0,
                           const int i1, const int j0, const int j1, const int b, const size_t max_RoIs_size)
: spu::module::Stateful(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), show_id(show_id), img_data(nullptr),
  video_writer(nullptr) {
    const std::string name = "Logger_frame";
    this->set_name(name);
    this->set_short_name(name);

    this->img_data = image_gs_alloc((i1 - i0) + 1, (j1 - j0) + 1);
    const size_t n_threads = 1;
    this->video_writer = video_writer_alloc_init(frames_path.c_str(), fra_start, n_threads, (i1 - i0) + 1,
                                                 (j1 - j0) + 1, PIXFMT_GRAY8, VCDC_FFMPEG_IO, 0, 0, NULL);

    const size_t img_n_rows = (i1 - i0) + 1 + 2 * b;
    const size_t img_n_cols = (j1 - j0) + 1 + 2 * b;

    auto &p = this->create_task("write");
    auto ps_in_labels = this->template create_2d_socket_in<uint32_t>(p, "in_labels", img_n_rows, img_n_cols);
    auto ps_in_RoIs_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs_basic",
                                                                     max_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);

    this->create_codelet(p, [ps_in_labels, ps_in_RoIs_basic, ps_in_n_RoIs]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_fra = static_cast<Logger_frame&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint32_t**   in_labels      =  t[ps_in_labels    ].get_2d_dataptr<const uint32_t   >(lgr_fra.b, lgr_fra.b);
        const RoI_basic_t* in_RoIs_basic  =  t[ps_in_RoIs_basic].get_dataptr   <const RoI_basic_t>();
        const uint32_t     in_n_RoIs      = *t[ps_in_n_RoIs    ].get_dataptr   <const uint32_t   >();

        image_gs_draw_labels(lgr_fra.img_data, in_labels, in_RoIs_basic, in_n_RoIs, lgr_fra.show_id);
        video_writer_save_frame(lgr_fra.video_writer, (const uint8_t**)image_gs_get_pixels_2d(lgr_fra.img_data));

        return spu::runtime::status_t::SUCCESS;
    });
}

Logger_frame::~Logger_frame() {
    image_gs_free(this->img_data);
    video_writer_free(this->video_writer);
}
