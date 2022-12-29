#include <nrc2.h>

#include "fmdt/Logger/Logger_frame.hpp"

Logger_frame::Logger_frame(const std::string frames_path, const std::string ext, const int show_id, const int i0,
                           const int i1, const int j0, const int j1, const int b, const size_t max_ROI_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), in_labels(nullptr), img_data(nullptr) {
    const std::string name = "Logger_frame";
    this->set_name(name);
    this->set_short_name(name);

    this->in_labels = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint32_t*)));
    this->in_labels -= i0 - b;

    this->img_data = tools_gray_img_alloc1((j1 - j0) + 1, (i1 - i0) + 1, frames_path.c_str(), ext.c_str(), show_id);

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("write");
    auto ps_in_labels = this->template create_socket_in<uint32_t>(p, "in_labels", socket_img_size);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    auto ps_in_ROI_id = this->template create_socket_in<uint32_t>(p, "in_ROI_id", max_ROI_size);
    auto ps_in_ROI_xmax = this->template create_socket_in<uint32_t>(p, "in_ROI_xmax", max_ROI_size);
    auto ps_in_ROI_ymin = this->template create_socket_in<uint32_t>(p, "in_ROI_ymin", max_ROI_size);
    auto ps_in_ROI_ymax = this->template create_socket_in<uint32_t>(p, "in_ROI_ymax", max_ROI_size);
    auto ps_in_n_ROI = this->template create_socket_in<uint32_t>(p, "in_n_ROI", 1);

    this->create_codelet(p, [ps_in_labels, ps_in_frame, ps_in_ROI_id, ps_in_ROI_xmax, ps_in_ROI_ymin, ps_in_ROI_ymax,
                             ps_in_n_ROI]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_fra = static_cast<Logger_frame&>(m);
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        const uint32_t* m_in_labels = static_cast<const uint32_t*>(t[ps_in_labels].get_dataptr());
        
        tools_linear_2d_nrc_ui32matrix(m_in_labels, lgr_fra.i0 - lgr_fra.b, lgr_fra.i1 + lgr_fra.b,
                                       lgr_fra.j0 - lgr_fra.b, lgr_fra.j1 + lgr_fra.b, lgr_fra.in_labels);

        _tools_gray_img_draw_labels(lgr_fra.img_data,
                                    lgr_fra.in_labels,
                                    static_cast<const uint32_t*>(t[ps_in_ROI_id].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_ROI_xmax].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_ROI_ymin].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_ROI_ymax].get_dataptr()),
                                    *static_cast<const uint32_t*>(t[ps_in_n_ROI].get_dataptr()));
        tools_gray_img_write1(lgr_fra.img_data, frame);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_frame::~Logger_frame() {
    free(this->in_labels + this->i0 - this->b);
    tools_gray_img_free(this->img_data);
}
