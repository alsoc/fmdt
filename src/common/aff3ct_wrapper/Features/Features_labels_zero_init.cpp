#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Features/Features_labels_zero_init.hpp"

Features_labels_zero_init::Features_labels_zero_init(const int i0, const int i1, const int j0, const int j1, const int b,
                                                     const size_t max_in_RoIs_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_in_RoIs_size(max_in_RoIs_size), in_out_labels(nullptr) {
    const std::string name = "Features_labels_zero_init";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data();

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("zinit");
    auto ps_in_out_labels = this->template create_socket_fwd<uint32_t>(p, "in_out_labels", socket_img_size);
    auto ps_in_RoIs_xmin = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmin", max_in_RoIs_size);
    auto ps_in_RoIs_xmax = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmax", max_in_RoIs_size);
    auto ps_in_RoIs_ymin = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymin", max_in_RoIs_size);
    auto ps_in_RoIs_ymax = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymax", max_in_RoIs_size);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);
    // this is a hack to lauch this task after the tracking (zinit[in_fake] = mergev3[status])
    /* auto ps_in_fake = */ this->template create_socket_in<int32_t>(p, "in_fake", 1);

    this->create_codelet(p, [ps_in_out_labels, ps_in_RoIs_xmin, ps_in_RoIs_xmax, ps_in_RoIs_ymin, ps_in_RoIs_ymax,
                             ps_in_n_RoIs]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lzi = static_cast<Features_labels_zero_init&>(m);

        uint32_t* m_in_out_labels = static_cast<uint32_t*>(t[ps_in_out_labels].get_dataptr());

        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_in_out_labels, lzi.i0 - lzi.b, lzi.i1 + lzi.b, lzi.j0 - lzi.b,
                                       lzi.j1 + lzi.b, (const uint32_t**)lzi.in_out_labels);

        void _features_labels_zero_init(const uint32_t* RoIs_xmin, const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin,
                                        const uint32_t* RoIs_ymax, const size_t n_RoIs, uint32_t** labels);

        _features_labels_zero_init(static_cast<const uint32_t*>(t[ps_in_RoIs_xmin].get_dataptr()),
                                   static_cast<const uint32_t*>(t[ps_in_RoIs_xmax].get_dataptr()),
                                   static_cast<const uint32_t*>(t[ps_in_RoIs_ymin].get_dataptr()),
                                   static_cast<const uint32_t*>(t[ps_in_RoIs_ymax].get_dataptr()),
                                   *static_cast<const uint32_t*>(t[ps_in_n_RoIs].get_dataptr()),
                                   lzi.in_out_labels);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

void Features_labels_zero_init::init_data() {
    this->in_out_labels = (uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint32_t*)));
    this->in_out_labels -= i0 - b;
}

Features_labels_zero_init::~Features_labels_zero_init() {
    free(this->in_out_labels + (this->i0 - this->b));
}

Features_labels_zero_init* Features_labels_zero_init::clone() const {
    auto m = new Features_labels_zero_init(*this);
    m->deep_copy(*this);
    return m;
}

void Features_labels_zero_init::deep_copy(const Features_labels_zero_init &m)
{
    Module::deep_copy(m);
    this->init_data();
}
