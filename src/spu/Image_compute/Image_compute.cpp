#include "fmdt/image/image_compute.h"
#include "fmdt/spu/Image_compute/Image_compute.hpp"

Image_compute::Image_compute(const size_t i0, const size_t i1, const size_t j0, const size_t j1, const size_t border)
: spu::module::Stateful(), i0(i0), i1(i1), j0(j0), j1(j1), border(border) {
    const std::string name = "Image_compute";
    this->set_name(name);
    this->set_short_name(name);

    const size_t height = i1 - i0 + 1 + 2 * border;
    const size_t width  = j1 - j0 + 1 + 2 * border;

    auto &convert = this->create_task("gray2rgb");
    auto converts_fwd_img_gray8  = this->template create_2d_socket_fwd<uint8_t>(convert, "fwd_img_gray8", height, width);
    auto converts_out_img_rgb24 = this->template create_2d_socket_out<uint8_t>(convert, "fwd_img_rgb24", height, width * 3);
    this->create_codelet(convert, [converts_fwd_img_gray8, converts_out_img_rgb24]
                            (spu::module::Module &m, spu::runtime::Task &t,const size_t frame_id) -> int {
        auto &img = static_cast<Image_compute&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const uint8_t** fwd_img_gray8 = t[converts_fwd_img_gray8].get_2d_dataptr<const uint8_t>(img.border, img.border);
              uint8_t** out_img_rgb24 = t[converts_out_img_rgb24].get_2d_dataptr<      uint8_t>(img.border, img.border);

        image_convert_gray8_to_rgb24(fwd_img_gray8, img.i0, img.i1, img.j0, img.j1, out_img_rgb24);

        return spu::runtime::status_t::SUCCESS;
    });

}

Image_compute* Image_compute::clone() const {
    Image_compute* ic = new Image_compute(*this);
    ic->deep_copy(*this);
    return ic;
}