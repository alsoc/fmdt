#include "fmdt/tools.h"

#include "fmdt/Logger/Logger_frame.hpp"

Logger_frame::Logger_frame(const std::string frames_path, const int i0, const int i1, const int j0, const int j1,
                           const int b)
: Module(), frames_path(frames_path), i0(i0), i1(i1), j0(j0), j1(j1), b(b), in_img(nullptr) {
    const std::string name = "Logger_frame";
    this->set_name(name);
    this->set_short_name(name);

    this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
    this->in_img -= i0 - b;

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("write");
    auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in_img", socket_img_size);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!frames_path.empty())
        tools_create_folder(frames_path.c_str());

    this->create_codelet(p, [ps_in_img, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_fra = static_cast<Logger_frame&>(m);
        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
        
        tools_linear_2d_nrc_ui8matrix(m_in_img, lgr_fra.i0 - lgr_fra.b, lgr_fra.i1 + lgr_fra.b, lgr_fra.j0 - lgr_fra.b, 
                                      lgr_fra.j1 + lgr_fra.b,  lgr_fra.in_img);

        if (!lgr_fra.frames_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.pgm", lgr_fra.frames_path.c_str(), frame);
            tools_save_frame_ui8matrix(file_path, lgr_fra.in_img, lgr_fra.i0, lgr_fra.i1, lgr_fra.j0, lgr_fra.j1);
        }

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_frame::~Logger_frame() {}
