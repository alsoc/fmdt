#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Features/Features_extractor.hpp"

Features_extractor::Features_extractor(const int i0, const int i1, const int j0, const int j1, const int b,
                                       const size_t max_RoI_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_RoI_size(max_RoI_size), in_img(nullptr) {
    const std::string name = "Features_extractor";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data();

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("extract");
    auto ps_in_img = this->template create_socket_in<uint32_t>(p, "in_img", socket_img_size);
    auto ps_in_n_RoI = this->template create_socket_in<uint32_t>(p, "in_n_RoI", 1);
    auto ps_out_RoI_id = this->template create_socket_out<uint32_t>(p, "out_RoI_id", max_RoI_size);
    auto ps_out_RoI_xmin = this->template create_socket_out<uint32_t>(p, "out_RoI_xmin", max_RoI_size);
    auto ps_out_RoI_xmax = this->template create_socket_out<uint32_t>(p, "out_RoI_xmax", max_RoI_size);
    auto ps_out_RoI_ymin = this->template create_socket_out<uint32_t>(p, "out_RoI_ymin", max_RoI_size);
    auto ps_out_RoI_ymax = this->template create_socket_out<uint32_t>(p, "out_RoI_ymax", max_RoI_size);
    auto ps_out_RoI_S = this->template create_socket_out<uint32_t>(p, "out_RoI_S", max_RoI_size);
    auto ps_out_RoI_Sx = this->template create_socket_out<uint32_t>(p, "out_RoI_Sx", max_RoI_size);
    auto ps_out_RoI_Sy = this->template create_socket_out<uint32_t>(p, "out_RoI_Sy", max_RoI_size);
    auto ps_out_RoI_x = this->template create_socket_out<float>(p, "out_RoI_x", max_RoI_size);
    auto ps_out_RoI_y = this->template create_socket_out<float>(p, "out_RoI_y", max_RoI_size);

    this->create_codelet(p, [ps_in_img, ps_in_n_RoI, ps_out_RoI_id, ps_out_RoI_xmin, ps_out_RoI_xmax, ps_out_RoI_ymin,
                             ps_out_RoI_ymax, ps_out_RoI_S, ps_out_RoI_Sx, ps_out_RoI_Sy, ps_out_RoI_x, ps_out_RoI_y]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &ext = static_cast<Features_extractor&>(m);
        
        const uint32_t* m_in_img = static_cast<const uint32_t*>(t[ps_in_img].get_dataptr());
        tools_linear_2d_nrc_ui32matrix(m_in_img, ext.i0 - ext.b, ext.i1 + ext.b, ext.j0 - ext.b, ext.j1 + ext.b, 
                                       ext.in_img);

        uint32_t n_RoI = *static_cast<uint32_t*>(t[ps_in_n_RoI].get_dataptr());

        _features_extract(ext.in_img, ext.i0, ext.i1, ext.j0, ext.j1,
                          static_cast<uint32_t*>(t[ps_out_RoI_id].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoI_xmin].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoI_xmax].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoI_ymin].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoI_ymax].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoI_S].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoI_Sx].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoI_Sy].get_dataptr()),
                          static_cast<float*>(t[ps_out_RoI_x].get_dataptr()),
                          static_cast<float*>(t[ps_out_RoI_y].get_dataptr()),
                          n_RoI);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

void Features_extractor::init_data() {
    this->in_img = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint32_t*)));
    this->in_img -= i0 - b;
}

Features_extractor::~Features_extractor() {
    free(this->in_img + (this->i0 - this->b));
}

Features_extractor* Features_extractor::clone() const {
    auto m = new Features_extractor(*this);
    m->deep_copy(*this);
    return m;
}

void Features_extractor::deep_copy(const Features_extractor &m)
{
    Module::deep_copy(m);
    this->init_data();
}
