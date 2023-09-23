#include "fmdt/visu/visu_io.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Visu/Visu.hpp"

Visu::Visu(const char* path, const size_t start, const size_t n_ffmpeg_threads, const int i0, const int i1,
           const int j0, const int j1, const int b, const enum pixfmt_e pixfmt, const enum video_codec_e codec_type,
           const uint8_t draw_track_id, const uint8_t draw_legend, const int win_play, const size_t buff_size,
           const size_t max_RoIs_size, const tracking_data_t* tracking_data)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), in_img(nullptr), tracking_data(tracking_data), visu_data(nullptr) {
    assert(tracking_data != NULL);

    const std::string name = "Visu";
    this->set_name(name);
    this->set_short_name(name);

    this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
    this->in_img -= i0 - b;

    this->visu_data = visu_alloc_init(path, start, n_ffmpeg_threads, (i1 - i0) + 1, (j1 - j0) + 1, pixfmt, codec_type,
                                      draw_track_id, draw_legend, win_play, buff_size, max_RoIs_size);

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("display");
    // auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);
    auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in_img", socket_img_size);
    auto ps_in_RoIs_xmin = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmin", max_RoIs_size);
    auto ps_in_RoIs_xmax = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmax", max_RoIs_size);
    auto ps_in_RoIs_ymin = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymin", max_RoIs_size);
    auto ps_in_RoIs_ymax = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymax", max_RoIs_size);
    auto ps_in_RoIs_x = this->template create_socket_in<float>(p, "in_RoIs_x", max_RoIs_size);
    auto ps_in_RoIs_y = this->template create_socket_in<float>(p, "in_RoIs_y", max_RoIs_size);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);
    // this is a hack to lauch this task after the tracking (visu[in_fake] = tracking[status])
    /* auto ps_in_fake = */ this->template create_socket_in<int32_t>(p, "in_fake", 1);

    this->create_codelet(p, [/* ps_in_frame, */ ps_in_img, ps_in_RoIs_xmin, ps_in_RoIs_xmax, ps_in_RoIs_ymin,
                             ps_in_RoIs_ymax, ps_in_RoIs_x, ps_in_RoIs_y, ps_in_n_RoIs]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &vis = static_cast<Visu&>(m);

        // const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());

        const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
        tools_linear_2d_nrc_ui8matrix(m_in_img, vis.i0 - vis.b, vis.i1 + vis.b, vis.j0 - vis.b, vis.j1 + vis.b,
                                      vis.in_img);

        _visu_display(vis.visu_data,
                      vis.in_img,
                      static_cast<const uint32_t*>(t[ps_in_RoIs_xmin].get_dataptr()),
                      static_cast<const uint32_t*>(t[ps_in_RoIs_xmax].get_dataptr()),
                      static_cast<const uint32_t*>(t[ps_in_RoIs_ymin].get_dataptr()),
                      static_cast<const uint32_t*>(t[ps_in_RoIs_ymax].get_dataptr()),
                      static_cast<const float*>(t[ps_in_RoIs_x].get_dataptr()),
                      static_cast<const float*>(t[ps_in_RoIs_y].get_dataptr()),
                      *static_cast<const uint32_t*>(t[ps_in_n_RoIs].get_dataptr()),
                      vis.tracking_data->tracks);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Visu::~Visu() {
    visu_free(this->visu_data);
}

void Visu::flush() {
    free(this->in_img + this->i0 - this->b);
    visu_flush(this->visu_data, this->tracking_data->tracks);
}
