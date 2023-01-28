#include "fmdt/features.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Features/Features_magnitude.hpp"

Features_magnitude::Features_magnitude(const int i0, const int i1, const int j0, const int j1, const int b,
                                       const size_t max_ROI_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_ROI_size(max_ROI_size), in_img(nullptr), in_labels(nullptr) {
    const std::string name = "Features_magnitude";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data();

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("compute");
    auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in_img", socket_img_size);
    auto ps_in_labels = this->template create_socket_in<uint32_t>(p, "in_labels", socket_img_size);

    auto ps_in_ROI_xmin = this->template create_socket_in<uint32_t>(p, "in_ROI_xmin", max_ROI_size);
    auto ps_in_ROI_xmax = this->template create_socket_in<uint32_t>(p, "in_ROI_xmax", max_ROI_size);
    auto ps_in_ROI_ymin = this->template create_socket_in<uint32_t>(p, "in_ROI_ymin", max_ROI_size);
    auto ps_in_ROI_ymax = this->template create_socket_in<uint32_t>(p, "in_ROI_ymax", max_ROI_size);
    auto ps_in_ROI_S = this->template create_socket_in<uint32_t>(p, "in_ROI_S", max_ROI_size);
    auto ps_in_n_ROI = this->template create_socket_in<uint32_t>(p, "in_n_ROI", 1);

    auto ps_out_ROI_magnitude = this->template create_socket_out<uint32_t>(p, "out_ROI_magnitude", max_ROI_size);

    this->create_codelet(p, [ps_in_img, ps_in_labels, ps_in_ROI_xmin, ps_in_ROI_xmax, ps_in_ROI_ymin, ps_in_ROI_ymax,
                             ps_in_ROI_S, ps_in_n_ROI, ps_out_ROI_magnitude]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &mgn = static_cast<Features_magnitude&>(m);
        const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
        const uint32_t* m_in_labels = static_cast<const uint32_t*>(t[ps_in_labels].get_dataptr());

        tools_linear_2d_nrc_ui8matrix(m_in_img, mgn.i0 - mgn.b, mgn.i1 + mgn.b, mgn.j0 - mgn.b, mgn.j1 + mgn.b,
                                      mgn.in_img);
        tools_linear_2d_nrc_ui32matrix(m_in_labels, mgn.i0 - mgn.b, mgn.i1 + mgn.b, mgn.j0 - mgn.b, mgn.j1 + mgn.b,
                                       mgn.in_labels);

        const uint32_t in_n_ROI = *static_cast<const uint32_t*>(t[ps_in_n_ROI].get_dataptr());

        _features_compute_magnitude(mgn.in_img,
                                    mgn.j1,
                                    mgn.i1,
                                    mgn.in_labels,
                                    static_cast<const uint32_t*>(t[ps_in_ROI_xmin].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_ROI_xmax].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_ROI_ymin].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_ROI_ymax].get_dataptr()),
                                    static_cast<const uint32_t*>(t[ps_in_ROI_S].get_dataptr()),
                                    static_cast<uint32_t*>(t[ps_out_ROI_magnitude].get_dataptr()),
                                    in_n_ROI);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

void Features_magnitude::init_data() {
    this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
    this->in_labels = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint32_t*)));
    this->in_img -= i0 - b;
    this->in_labels -= i0 - b;
}

Features_magnitude::~Features_magnitude() {
    free(this->in_img + (this->i0 - this->b));
    free(this->in_labels + (this->i0 - this->b));
}

Features_magnitude* Features_magnitude::clone() const {
    auto m = new Features_magnitude(*this);
    m->deep_copy(*this);
    return m;
}

void Features_magnitude::deep_copy(const Features_magnitude &m) {
    Module::deep_copy(m);
    this->init_data();
}
