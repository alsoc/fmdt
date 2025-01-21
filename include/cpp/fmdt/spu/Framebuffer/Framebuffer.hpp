/*!
 * \file
 * \brief C++ wrapper for Framebuffer.
 */

#pragma once

#include <stdint.h>
#include <streampu.hpp>

#include "fmdt/framebuffer/framebuffer_struct.h"
#include "fmdt/spu/Tracking/Tracking.hpp"

namespace fb {
enum class tsk : size_t { bufferize, SIZE };
namespace sck {
enum class bufferize : size_t { in_frame_id, in_img_rgb24, in_RoIs_basic, in_n_RoIs, status };
}
} // namespace fb

class Framebuffer : public spu::module::Stateful {
  protected:
    framebuffer_data_t* framebuffer;
    size_t border;

  public:
    Framebuffer(const size_t size, const size_t frame_height, const size_t frame_width, const size_t border,
                const size_t frame_skip, const enum pixfmt_e frame_pixfmt, const size_t max_RoIs_size);
    virtual ~Framebuffer();
    void flush();
    void register_display(const size_t n_ffmpeg_threads, const enum video_codec_e codec_type);
    void register_write(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                        const enum video_codec_e codec_type);
    void register_extractor(const char* path, const size_t n_writers, const size_t n_threads,
                            const enum video_codec_e codec, Tracking& tracking);
    void register_draw_frame_id();
    void register_draw_legend(const bool* draw_validation);
    void register_draw_boxes(Tracking& tracking, const int* draw_id);

    inline spu::runtime::Task& operator[](const fb::tsk t);
    inline spu::runtime::Socket& operator[](const fb::sck::bufferize s);
    inline spu::runtime::Socket& operator[](const std::string& tsk_sck);
};

#include "fmdt/spu/Framebuffer/Framebuffer.hxx"
