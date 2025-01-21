#include "fmdt/framebuffer/framebuffer_io.h"
#include "fmdt/framebuffer/frame_compute.h"

#include "fmdt/tools.h"
#include "fmdt/visu/visu_io.h"

#include "fmdt/spu/Framebuffer/Framebuffer.hpp"
#include "fmdt/spu/Tracking/Tracking.hpp"

Framebuffer::Framebuffer(const size_t size, const size_t frame_height, const size_t frame_width, const size_t border,
                         const size_t frame_skip, const enum pixfmt_e frame_pixfmt, const size_t max_RoIs_size)
    : spu::module::Stateful() {

    const std::string name = "Framebuffer";
    this->set_name(name);
    this->set_short_name(name);

    this->framebuffer =
        framebuffer_alloc_init(size, frame_height, frame_width, frame_skip, frame_pixfmt, max_RoIs_size);
    this->border = border;

    const size_t pixsize = image_get_pixsize(frame_pixfmt);
    const size_t height = frame_height + 2 * border;
    const size_t width = frame_width + 2 * border;

    auto& p = this->create_task("bufferize");
    auto ps_in_frame_id = this->template create_socket_in<uint32_t>(p, "in_frame_id", 1);
    auto ps_in_img_rgb24 = this->template create_2d_socket_in<uint8_t>(p, "in_img_rgb24", height, width * pixsize);
    auto ps_in_RoIs_basic =
        this->template create_socket_in<uint8_t>(p, "in_RoIs_basic", max_RoIs_size * sizeof(RoI_basic_t));
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);

    this->create_codelet(p,
                         [ps_in_frame_id, ps_in_img_rgb24, ps_in_RoIs_basic,
                          ps_in_n_RoIs](spu::module::Module& m, spu::runtime::Task& t, const size_t frame_id) -> int {
                             auto& fb = static_cast<Framebuffer&>(m);
                             const uint32_t in_frame_id = *t[ps_in_frame_id].get_dataptr<const uint32_t>();
                             const uint8_t** in_img_rgb24 =
                                 t[ps_in_img_rgb24].get_2d_dataptr<const uint8_t>(fb.border, fb.border);
                             const RoI_basic_t* in_RoIs_basic = t[ps_in_RoIs_basic].get_dataptr<const RoI_basic_t>();
                             const uint32_t in_n_RoIs = *t[ps_in_n_RoIs].get_dataptr<const uint32_t>();

                             framebuffer_push(fb.framebuffer, in_frame_id, in_img_rgb24, in_RoIs_basic, in_n_RoIs);

                             return spu::runtime::status_t::SUCCESS;
                         });
}

void Framebuffer::register_display(const size_t n_ffmpeg_threads, const enum video_codec_e codec_type) {
    frame_write_action_register(this->framebuffer, NULL, 0, n_ffmpeg_threads, 1, codec_type);
}

void Framebuffer::register_write(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                                 const enum video_codec_e codec_type) {
    frame_write_action_register(this->framebuffer, path, start, n_ffmpeg_threads, 0, codec_type);
}

void Framebuffer::register_extractor(const char* path, const size_t n_writers, const size_t n_threads,
                                     const enum video_codec_e codec, Tracking& tracking) {
    frame_extract_action_register(this->framebuffer, path, n_writers, n_threads, codec, tracking.get_data());
}

void Framebuffer::register_draw_frame_id() { frame_draw_id_action_register(this->framebuffer); }

void Framebuffer::register_draw_legend(const bool* draw_validation) {
    frame_draw_legend_action_register(this->framebuffer, (const int*)draw_validation);
}

void Framebuffer::register_draw_boxes(Tracking& tracking, const int* draw_id) {
    frame_draw_boxes_action_register(this->framebuffer, tracking.get_data(), draw_id);
}

Framebuffer::~Framebuffer() { framebuffer_free(this->framebuffer); }

void Framebuffer::flush() { framebuffer_flush(this->framebuffer); }
