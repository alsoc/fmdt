#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI_v3.hpp"

Features_merger_CCL_HI_v3::Features_merger_CCL_HI_v3(const int i0, const int i1, const int j0, const int j1, const int b,
                                                     const uint32_t S_min, const uint32_t S_max,
                                                     const uint8_t threshold_high, const uint8_t no_labels_zeros_init,
                                                     const size_t max_in_RoIs_size, const size_t max_out_RoIs_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), S_min(S_min), S_max(S_max), threshold_high(threshold_high),
  no_labels_zeros_init(no_labels_zeros_init), max_in_RoIs_size(max_in_RoIs_size), max_out_RoIs_size(max_out_RoIs_size),
  in_labels(nullptr), in_img(nullptr), out_labels(nullptr), tmp_in_RoIs_id(nullptr) {
    const std::string name = "Features_merger_CCL_HI_v3";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data();

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p = this->create_task("merge");
    auto ps_in_labels = this->template create_socket_in<uint32_t>(p, "in_labels", socket_img_size);
    auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in_img", socket_img_size);

    auto ps_in_RoIs_id = this->template create_socket_in<uint32_t>(p, "in_RoIs_id", max_in_RoIs_size);
    auto ps_in_RoIs_xmin = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmin", max_in_RoIs_size);
    auto ps_in_RoIs_xmax = this->template create_socket_in<uint32_t>(p, "in_RoIs_xmax", max_in_RoIs_size);
    auto ps_in_RoIs_ymin = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymin", max_in_RoIs_size);
    auto ps_in_RoIs_ymax = this->template create_socket_in<uint32_t>(p, "in_RoIs_ymax", max_in_RoIs_size);
    auto ps_in_RoIs_S = this->template create_socket_in<uint32_t>(p, "in_RoIs_S", max_in_RoIs_size);
    auto ps_in_RoIs_Sx = this->template create_socket_in<uint32_t>(p, "in_RoIs_Sx", max_in_RoIs_size);
    auto ps_in_RoIs_Sy = this->template create_socket_in<uint32_t>(p, "in_RoIs_Sy", max_in_RoIs_size);
    auto ps_in_RoIs_Sx2 = this->template create_socket_in<uint64_t>(p, "in_RoIs_Sx2", max_in_RoIs_size);
    auto ps_in_RoIs_Sy2 = this->template create_socket_in<uint64_t>(p, "in_RoIs_Sy2", max_in_RoIs_size);
    auto ps_in_RoIs_Sxy = this->template create_socket_in<uint64_t>(p, "in_RoIs_Sxy", max_in_RoIs_size);
    auto ps_in_RoIs_x = this->template create_socket_in<float>(p, "in_RoIs_x", max_in_RoIs_size);
    auto ps_in_RoIs_y = this->template create_socket_in<float>(p, "in_RoIs_y", max_in_RoIs_size);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);

    auto ps_out_RoIs_id = this->template create_socket_out<uint32_t>(p, "out_RoIs_id", max_out_RoIs_size);
    auto ps_out_RoIs_xmin = this->template create_socket_out<uint32_t>(p, "out_RoIs_xmin", max_out_RoIs_size);
    auto ps_out_RoIs_xmax = this->template create_socket_out<uint32_t>(p, "out_RoIs_xmax", max_out_RoIs_size);
    auto ps_out_RoIs_ymin = this->template create_socket_out<uint32_t>(p, "out_RoIs_ymin", max_out_RoIs_size);
    auto ps_out_RoIs_ymax = this->template create_socket_out<uint32_t>(p, "out_RoIs_ymax", max_out_RoIs_size);
    auto ps_out_RoIs_S = this->template create_socket_out<uint32_t>(p, "out_RoIs_S", max_out_RoIs_size);
    auto ps_out_RoIs_Sx = this->template create_socket_out<uint32_t>(p, "out_RoIs_Sx", max_out_RoIs_size);
    auto ps_out_RoIs_Sy = this->template create_socket_out<uint32_t>(p, "out_RoIs_Sy", max_out_RoIs_size);
    auto ps_out_RoIs_Sx2 = this->template create_socket_out<uint64_t>(p, "out_RoIs_Sx2", max_out_RoIs_size);
    auto ps_out_RoIs_Sy2 = this->template create_socket_out<uint64_t>(p, "out_RoIs_Sy2", max_out_RoIs_size);
    auto ps_out_RoIs_Sxy = this->template create_socket_out<uint64_t>(p, "out_RoIs_Sxy", max_out_RoIs_size);
    auto ps_out_RoIs_x = this->template create_socket_out<float>(p, "out_RoIs_x", max_out_RoIs_size);
    auto ps_out_RoIs_y = this->template create_socket_out<float>(p, "out_RoIs_y", max_out_RoIs_size);
    auto ps_out_n_RoIs = this->template create_socket_out<uint32_t>(p, "out_n_RoIs", 1);

    auto ps_out_labels = this->template create_socket_out<uint32_t>(p, "out_labels", socket_img_size);

    this->create_codelet(p, [ps_in_labels, ps_in_img, ps_in_RoIs_id, ps_in_RoIs_xmin, ps_in_RoIs_xmax,
                             ps_in_RoIs_ymin, ps_in_RoIs_ymax, ps_in_RoIs_S, ps_in_RoIs_Sx, ps_in_RoIs_Sy,
                             ps_in_RoIs_Sx2, ps_in_RoIs_Sy2, ps_in_RoIs_Sxy, ps_in_RoIs_x, ps_in_RoIs_y, ps_in_n_RoIs,
                             ps_out_RoIs_id, ps_out_RoIs_xmin, ps_out_RoIs_xmax, ps_out_RoIs_ymin, ps_out_RoIs_ymax,
                             ps_out_RoIs_S, ps_out_RoIs_Sx, ps_out_RoIs_Sy, ps_out_RoIs_Sx2, ps_out_RoIs_Sy2,
                             ps_out_RoIs_Sxy, ps_out_RoIs_x, ps_out_RoIs_y, ps_out_n_RoIs, ps_out_labels]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &mrg = static_cast<Features_merger_CCL_HI_v3&>(m);
        const uint32_t* m_in_labels = static_cast<const uint32_t*>(t[ps_in_labels].get_dataptr());
        const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
        uint32_t* m_out_labels = static_cast<uint32_t*>(t[ps_out_labels].get_dataptr());

        tools_linear_2d_nrc_ui32matrix(m_in_labels, mrg.i0 - mrg.b, mrg.i1 + mrg.b, mrg.j0 - mrg.b, mrg.j1 + mrg.b,
                                       mrg.in_labels);
        tools_linear_2d_nrc_ui8matrix(m_in_img, mrg.i0 - mrg.b, mrg.i1 + mrg.b, mrg.j0 - mrg.b, mrg.j1 + mrg.b,
                                      mrg.in_img);
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_out_labels, mrg.i0 - mrg.b, mrg.i1 + mrg.b, mrg.j0 - mrg.b,
                                      mrg.j1 + mrg.b, (const uint32_t**)mrg.out_labels);

        const uint32_t in_n_RoIs = *static_cast<const uint32_t*>(t[ps_in_n_RoIs].get_dataptr());

        std::copy_n(static_cast<const uint32_t*>(t[ps_in_RoIs_id].get_dataptr()), in_n_RoIs, mrg.tmp_in_RoIs_id);

        _features_merge_CCL_HI_v3(mrg.in_labels, mrg.in_img, mrg.out_labels, mrg.i0, mrg.i1, mrg.j0, mrg.j1,
                                  mrg.tmp_in_RoIs_id,
                                  static_cast<const uint32_t*>(t[ps_in_RoIs_xmin].get_dataptr()),
                                  static_cast<const uint32_t*>(t[ps_in_RoIs_xmax].get_dataptr()),
                                  static_cast<const uint32_t*>(t[ps_in_RoIs_ymin].get_dataptr()),
                                  static_cast<const uint32_t*>(t[ps_in_RoIs_ymax].get_dataptr()),
                                  static_cast<const uint32_t*>(t[ps_in_RoIs_S].get_dataptr()),
                                  in_n_RoIs,
                                  mrg.S_min, mrg.S_max, mrg.threshold_high, mrg.no_labels_zeros_init);

        size_t out_n_RoIs = _features_shrink_basic(mrg.tmp_in_RoIs_id,
                                                   static_cast<const uint32_t*>(t[ps_in_RoIs_xmin].get_dataptr()),
                                                   static_cast<const uint32_t*>(t[ps_in_RoIs_xmax].get_dataptr()),
                                                   static_cast<const uint32_t*>(t[ps_in_RoIs_ymin].get_dataptr()),
                                                   static_cast<const uint32_t*>(t[ps_in_RoIs_ymax].get_dataptr()),
                                                   static_cast<const uint32_t*>(t[ps_in_RoIs_S].get_dataptr()),
                                                   static_cast<const uint32_t*>(t[ps_in_RoIs_Sx].get_dataptr()),
                                                   static_cast<const uint32_t*>(t[ps_in_RoIs_Sy].get_dataptr()),
                                                   static_cast<const uint64_t*>(t[ps_in_RoIs_Sx2].get_dataptr()),
                                                   static_cast<const uint64_t*>(t[ps_in_RoIs_Sy2].get_dataptr()),
                                                   static_cast<const uint64_t*>(t[ps_in_RoIs_Sxy].get_dataptr()),
                                                   static_cast<const float*>(t[ps_in_RoIs_x].get_dataptr()),
                                                   static_cast<const float*>(t[ps_in_RoIs_y].get_dataptr()),
                                                   in_n_RoIs,
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
                                                   static_cast<float*>(t[ps_out_RoIs_y].get_dataptr()));

        *static_cast<uint32_t*>(t[ps_out_n_RoIs].get_dataptr()) = (uint32_t)out_n_RoIs;

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

void Features_merger_CCL_HI_v3::init_data() {
    this->tmp_in_RoIs_id = (uint32_t*)malloc(this->max_in_RoIs_size * sizeof(uint32_t));
    this->in_labels = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint32_t*)));
    this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
    this->out_labels = (uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint32_t*)));
    this->in_labels -= i0 - b;
    this->in_img -= i0 - b;
    this->out_labels -= i0 - b;
}

Features_merger_CCL_HI_v3::~Features_merger_CCL_HI_v3() {
    free(this->tmp_in_RoIs_id);
    free(this->in_labels + (this->i0 - this->b));
    free(this->in_img + (this->i0 - this->b));
    free(this->out_labels + (this->i0 - this->b));
}

Features_merger_CCL_HI_v3* Features_merger_CCL_HI_v3::clone() const {
    auto m = new Features_merger_CCL_HI_v3(*this);
    m->deep_copy(*this);
    return m;
}

void Features_merger_CCL_HI_v3::deep_copy(const Features_merger_CCL_HI_v3 &m)
{
    Module::deep_copy(m);
    this->init_data();
}
