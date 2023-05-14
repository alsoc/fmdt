#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Features/Features_extractor.hpp"

Features_extractor::Features_extractor(const int i0, const int i1, const int j0, const int j1, const int b,
                                       const size_t max_RoIs_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_RoIs_size(max_RoIs_size), in_img(nullptr) {
    const std::string name = "Features_extractor";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data();

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("extract");
    auto ps_in_img = this->template create_socket_in<uint32_t>(p, "in_img", socket_img_size);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);
    auto ps_out_RoIs_id = this->template create_socket_out<uint32_t>(p, "out_RoIs_id", max_RoIs_size);
    auto ps_out_RoIs_xmin = this->template create_socket_out<uint32_t>(p, "out_RoIs_xmin", max_RoIs_size);
    auto ps_out_RoIs_xmax = this->template create_socket_out<uint32_t>(p, "out_RoIs_xmax", max_RoIs_size);
    auto ps_out_RoIs_ymin = this->template create_socket_out<uint32_t>(p, "out_RoIs_ymin", max_RoIs_size);
    auto ps_out_RoIs_ymax = this->template create_socket_out<uint32_t>(p, "out_RoIs_ymax", max_RoIs_size);
    auto ps_out_RoIs_S = this->template create_socket_out<uint32_t>(p, "out_RoIs_S", max_RoIs_size);
    auto ps_out_RoIs_Sx = this->template create_socket_out<uint32_t>(p, "out_RoIs_Sx", max_RoIs_size);
    auto ps_out_RoIs_Sy = this->template create_socket_out<uint32_t>(p, "out_RoIs_Sy", max_RoIs_size);
    auto ps_out_RoIs_Sx2 = this->template create_socket_out<uint64_t>(p, "out_RoIs_Sx2", max_RoIs_size);
    auto ps_out_RoIs_Sy2 = this->template create_socket_out<uint64_t>(p, "out_RoIs_Sy2", max_RoIs_size);
    auto ps_out_RoIs_Sxy = this->template create_socket_out<uint64_t>(p, "out_RoIs_Sxy", max_RoIs_size);
    auto ps_out_RoIs_x = this->template create_socket_out<float>(p, "out_RoIs_x", max_RoIs_size);
    auto ps_out_RoIs_y = this->template create_socket_out<float>(p, "out_RoIs_y", max_RoIs_size);

    this->create_codelet(p, [ps_in_img, ps_in_n_RoIs, ps_out_RoIs_id, ps_out_RoIs_xmin, ps_out_RoIs_xmax,
                             ps_out_RoIs_ymin, ps_out_RoIs_ymax, ps_out_RoIs_S, ps_out_RoIs_Sx, ps_out_RoIs_Sy,
                             ps_out_RoIs_Sx2, ps_out_RoIs_Sy2, ps_out_RoIs_Sxy, ps_out_RoIs_x, ps_out_RoIs_y]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &ext = static_cast<Features_extractor&>(m);
        
        const uint32_t* m_in_img = static_cast<const uint32_t*>(t[ps_in_img].get_dataptr());
        tools_linear_2d_nrc_ui32matrix(m_in_img, ext.i0 - ext.b, ext.i1 + ext.b, ext.j0 - ext.b, ext.j1 + ext.b, 
                                       ext.in_img);

        uint32_t n_RoIs = *static_cast<uint32_t*>(t[ps_in_n_RoIs].get_dataptr());
        assert(n_RoIs <= ext.max_RoIs_size);
        _features_extract(ext.in_img, ext.i0, ext.i1, ext.j0, ext.j1,
                          static_cast<uint32_t*>(t[ps_out_RoIs_id].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoIs_xmin].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoIs_xmax].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoIs_ymin].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoIs_ymax].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoIs_S].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoIs_Sx].get_dataptr()),
                          static_cast<uint32_t*>(t[ps_out_RoIs_Sy].get_dataptr()),
                          static_cast<uint64_t*>(t[ps_out_RoIs_Sx2].get_dataptr()),
                          static_cast<uint64_t*>(t[ps_out_RoIs_Sy2].get_dataptr()),
                          static_cast<uint64_t*>(t[ps_out_RoIs_Sxy].get_dataptr()),
                          static_cast<float*>(t[ps_out_RoIs_x].get_dataptr()),
                          static_cast<float*>(t[ps_out_RoIs_y].get_dataptr()),
                          n_RoIs);

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
