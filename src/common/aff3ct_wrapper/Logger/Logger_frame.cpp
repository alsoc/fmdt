#include <nrc2.h>

#include "fmdt/tools.h"
#include "fmdt/image/image_compute.h"
#include "fmdt/video/video_io.h"

#include "fmdt/aff3ct_wrapper/Logger/Logger_frame.hpp"

Logger_frame::Logger_frame(const std::string frames_path, const size_t fra_start, const int show_id, const int i0,
                           const int i1, const int j0, const int j1, const int b, const size_t max_RoIs_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), show_id(show_id), in_labels(nullptr), img_data(nullptr),
  video_writer(nullptr) {
    const std::string name = "Logger_frame";
    this->set_name(name);
    this->set_short_name(name);

    this->in_labels = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint32_t*)));
    this->in_labels -= i0 - b;

    this->img_data = image_gs_alloc((j1 - j0) + 1, (i1 - i0) + 1);
    const size_t n_threads = 1;
    this->video_writer = video_writer_alloc_init(frames_path.c_str(), fra_start, n_threads, (i1 - i0) + 1,
                                                 (j1 - j0) + 1, PIXFMT_GRAY);

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("write");
    auto ps_in_labels = this->template create_socket_in<uint32_t>(p, "in_labels", socket_img_size);

    auto ps_in_RoIs_id = this->template create_socket_in<uint32_t>(p, "in_RoIs_id", max_RoIs_size);
    auto ps_in_RoIs_xmax = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmax", max_RoIs_size);
    auto ps_in_RoIs_ymin = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymin", max_RoIs_size);
    auto ps_in_RoIs_ymax = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymax", max_RoIs_size);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);

    this->create_codelet(p, [ps_in_labels, ps_in_RoIs_id, ps_in_RoIs_xmax, ps_in_RoIs_ymin, ps_in_RoIs_ymax,
                             ps_in_n_RoIs]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_fra = static_cast<Logger_frame&>(m);
        const uint32_t* m_in_labels = static_cast<const uint32_t*>(t[ps_in_labels].get_dataptr());

        tools_linear_2d_nrc_ui32matrix(m_in_labels, lgr_fra.i0 - lgr_fra.b, lgr_fra.i1 + lgr_fra.b,
                                       lgr_fra.j0 - lgr_fra.b, lgr_fra.j1 + lgr_fra.b, lgr_fra.in_labels);

        _image_gs_draw_labels(lgr_fra.img_data,
                              lgr_fra.in_labels,
                              static_cast<const uint32_t*>(t[ps_in_RoIs_id].get_dataptr()),
                              static_cast<const uint32_t*>(t[ps_in_RoIs_xmax].get_dataptr()),
                              static_cast<const uint32_t*>(t[ps_in_RoIs_ymin].get_dataptr()),
                              static_cast<const uint32_t*>(t[ps_in_RoIs_ymax].get_dataptr()),
                              *static_cast<const uint32_t*>(t[ps_in_n_RoIs].get_dataptr()),
                              lgr_fra.show_id);
        video_writer_save_frame(lgr_fra.video_writer, (const uint8_t**)image_gs_get_pixels_2d(lgr_fra.img_data));

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_frame::~Logger_frame() {
    free(this->in_labels + this->i0 - this->b);
    image_gs_free(this->img_data);
    video_writer_free(this->video_writer);
}
