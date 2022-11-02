#include "fmdt/images.h"

#include "fmdt/Images/Images.hpp"

Images::Images(const std::string path, const size_t frame_start, const size_t frame_end, const size_t frame_skip,
               const int b)
: Module(), b(b), images(nullptr), out_img(nullptr), done(false) {
    const std::string name = "Images";
    this->set_name(name);
    this->set_short_name(name);

    this->images = images_init_from_path(path.c_str(), frame_start, frame_end, frame_skip);

    this->out_img = (uint8_t**)malloc((size_t)(((this->images->i1 - this->images->i0) + 1 + 2 * b) * sizeof(uint8_t*)));
    this->out_img -= this->images->i0 - b;

    auto socket_size = ((this->images->i1 - this->images->i0) + 1 + 2 * b) *
                       ((this->images->j1 - this->images->j0) + 1 + 2 * b);

    auto &p = this->create_task("generate");
    auto ps_out_img = this->template create_socket_out<uint8_t>(p, "out_img", socket_size);
    auto ps_out_frame = this->template create_socket_out<uint32_t>(p, "out_frame", 1);

    this->create_codelet(p, [ps_out_img, ps_out_frame]
                            (aff3ct::module::Module &m, aff3ct::runtime::Task &t,const size_t frame_id) -> int {
        auto &img = static_cast<Images&>(m);
        uint8_t* m_out_img = static_cast<uint8_t*>(t[ps_out_img].get_dataptr());
        img.out_img[img.images->i0 - img.b] = m_out_img - (img.images->j0 - img.b);
        for (int i = img.images->i0 - img.b + 1; i <= img.images->i1 + img.b; i++)
            img.out_img[i] = img.out_img[i - 1] + ((img.images->j1 - img.images->j0) + 1 + 2 * img.b);

        *static_cast<uint32_t*>(t[ps_out_frame].get_dataptr()) = img.images->frame_current + img.images->frame_start;
        int ret = images_get_next_frame(img.images, img.out_img);
        img.done = ret ? false : true;
        if (img.done)
            throw aff3ct::tools::processing_aborted(__FILE__, __LINE__, __func__);
        return ret ? aff3ct::runtime::status_t::SUCCESS : aff3ct::runtime::status_t::FAILURE_STOP;
    });
}

Images::~Images() {
    free(this->out_img + this->images->i0 - this->b);
    images_free(this->images);
}

bool Images::is_done() const {
    return this->done;
}
