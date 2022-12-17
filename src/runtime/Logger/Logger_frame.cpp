#include <nrc2.h>

#include "fmdt/tools.h"

#include "fmdt/Logger/Logger_frame.hpp"

Logger_frame::Logger_frame(const std::string frames_path, const int i0, const int i1, const int j0, const int j1,
                           const int b)
: Module(), frames_path(frames_path), i0(i0), i1(i1), j0(j0), j1(j1), b(b), in_labels(nullptr), img_tmp(nullptr) {
    const std::string name = "Logger_frame";
    this->set_name(name);
    this->set_short_name(name);

    this->in_labels = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint32_t*)));
    this->in_labels -= i0 - b;

    this->img_tmp = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("write");
    auto ps_in_labels = this->template create_socket_in<uint32_t>(p, "in_labels", socket_img_size);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!frames_path.empty())
        tools_create_folder(frames_path.c_str());

    this->create_codelet(p, [ps_in_labels, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_fra = static_cast<Logger_frame&>(m);
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        const uint32_t* m_in_labels = static_cast<const uint32_t*>(t[ps_in_labels].get_dataptr());
        
        tools_linear_2d_nrc_ui32matrix(m_in_labels, lgr_fra.i0 - lgr_fra.b, lgr_fra.i1 + lgr_fra.b,
                                       lgr_fra.j0 - lgr_fra.b, lgr_fra.j1 + lgr_fra.b,  lgr_fra.in_labels);

        if (!lgr_fra.frames_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.pgm", lgr_fra.frames_path.c_str(), frame);
            // convert labels to black & white image: white if there is a CC, black otherwise
            for (int i = lgr_fra.i0; i <= lgr_fra.i1; i++)
                for (int j = lgr_fra.j0; j <= lgr_fra.j1; j++)
                    lgr_fra.img_tmp[i][j] = (lgr_fra.in_labels[i][j] == 0) ? 0 : 255;
            SavePGM_ui8matrix((uint8**)lgr_fra.img_tmp, lgr_fra.i0, lgr_fra.i1, lgr_fra.j0, lgr_fra.j1, file_path);
        }

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_frame::~Logger_frame() {
    free(this->in_labels + this->i0 - this->b);
    free_ui8matrix(this->img_tmp, this->i0 - this->b, this->i1 + this->b, this->j0 - this->b, this->j1 + this->b);
}
