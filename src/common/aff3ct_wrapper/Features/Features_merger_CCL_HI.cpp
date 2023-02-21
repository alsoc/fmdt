#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Features/Features_merger_CCL_HI.hpp"

Features_merger_CCL_HI::Features_merger_CCL_HI(const int i0, const int i1, const int j0, const int j1, const int b,
                                               const uint32_t S_min, const uint32_t S_max, const uint8_t threshold_high,
                                               const uint8_t no_labels_zeros_init, const size_t max_in_RoIs_size,
                                               const size_t max_out_RoIs_size)
: Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), S_min(S_min), S_max(S_max), threshold_high(threshold_high),
  no_labels_zeros_init(no_labels_zeros_init), max_in_RoIs_size(max_in_RoIs_size), max_out_RoIs_size(max_out_RoIs_size),
  in_labels(nullptr), in_img_HI(nullptr), out_labels(nullptr), tmp_in_RoIs_id(nullptr) {
    const std::string name = "Features_merger_CCL_HI";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data();

    auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

    auto &p1 = this->create_task("merge_v2");
    auto p1s_in_labels = this->template create_socket_in<uint32_t>(p1, "in_labels", socket_img_size);
    auto p1s_in_img_HI = this->template create_socket_in<uint8_t>(p1, "in_img_HI", socket_img_size);

    auto p1s_in_RoIs_id = this->template create_socket_in<uint32_t>(p1, "in_RoIs_id", max_in_RoIs_size);
    auto p1s_in_RoIs_xmin = this->template create_socket_in<uint32_t>(p1, "in_RoIs_xmin", max_in_RoIs_size);
    auto p1s_in_RoIs_xmax = this->template create_socket_in<uint32_t>(p1, "in_RoIs_xmax", max_in_RoIs_size);
    auto p1s_in_RoIs_ymin = this->template create_socket_in<uint32_t>(p1, "in_RoIs_ymin", max_in_RoIs_size);
    auto p1s_in_RoIs_ymax = this->template create_socket_in<uint32_t>(p1, "in_RoIs_ymax", max_in_RoIs_size);
    auto p1s_in_RoIs_S = this->template create_socket_in<uint32_t>(p1, "in_RoIs_S", max_in_RoIs_size);
    auto p1s_in_RoIs_Sx = this->template create_socket_in<uint32_t>(p1, "in_RoIs_Sx", max_in_RoIs_size);
    auto p1s_in_RoIs_Sy = this->template create_socket_in<uint32_t>(p1, "in_RoIs_Sy", max_in_RoIs_size);
    auto p1s_in_RoIs_x = this->template create_socket_in<float>(p1, "in_RoIs_x", max_in_RoIs_size);
    auto p1s_in_RoIs_y = this->template create_socket_in<float>(p1, "in_RoIs_y", max_in_RoIs_size);
    auto p1s_in_n_RoIs = this->template create_socket_in<uint32_t>(p1, "in_n_RoIs", 1);

    auto p1s_out_RoIs_id = this->template create_socket_out<uint32_t>(p1, "out_RoIs_id", max_out_RoIs_size);
    auto p1s_out_RoIs_xmin = this->template create_socket_out<uint32_t>(p1, "out_RoIs_xmin", max_out_RoIs_size);
    auto p1s_out_RoIs_xmax = this->template create_socket_out<uint32_t>(p1, "out_RoIs_xmax", max_out_RoIs_size);
    auto p1s_out_RoIs_ymin = this->template create_socket_out<uint32_t>(p1, "out_RoIs_ymin", max_out_RoIs_size);
    auto p1s_out_RoIs_ymax = this->template create_socket_out<uint32_t>(p1, "out_RoIs_ymax", max_out_RoIs_size);
    auto p1s_out_RoIs_S = this->template create_socket_out<uint32_t>(p1, "out_RoIs_S", max_out_RoIs_size);
    auto p1s_out_RoIs_Sx = this->template create_socket_out<uint32_t>(p1, "out_RoIs_Sx", max_out_RoIs_size);
    auto p1s_out_RoIs_Sy = this->template create_socket_out<uint32_t>(p1, "out_RoIs_Sy", max_out_RoIs_size);
    auto p1s_out_RoIs_x = this->template create_socket_out<float>(p1, "out_RoIs_x", max_out_RoIs_size);
    auto p1s_out_RoIs_y = this->template create_socket_out<float>(p1, "out_RoIs_y", max_out_RoIs_size);
    auto p1s_out_n_RoIs = this->template create_socket_out<uint32_t>(p1, "out_n_RoIs", 1);

    auto p1s_out_labels = this->template create_socket_out<uint32_t>(p1, "out_labels", socket_img_size);

    this->create_codelet(p1, [p1s_in_labels, p1s_in_img_HI, p1s_in_RoIs_id, p1s_in_RoIs_xmin, p1s_in_RoIs_xmax,
                              p1s_in_RoIs_ymin, p1s_in_RoIs_ymax, p1s_in_RoIs_S, p1s_in_RoIs_Sx, p1s_in_RoIs_Sy,
                              p1s_in_RoIs_x, p1s_in_RoIs_y, p1s_in_n_RoIs, p1s_out_RoIs_id, p1s_out_RoIs_xmin,
                              p1s_out_RoIs_xmax, p1s_out_RoIs_ymin, p1s_out_RoIs_ymax, p1s_out_RoIs_S, p1s_out_RoIs_Sx,
                              p1s_out_RoIs_Sy, p1s_out_RoIs_x, p1s_out_RoIs_y, p1s_out_n_RoIs, p1s_out_labels]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &mrg = static_cast<Features_merger_CCL_HI&>(m);
        const uint32_t* m_in_labels = static_cast<const uint32_t*>(t[p1s_in_labels].get_dataptr());
        const uint8_t* m_in_img_HI = static_cast<const uint8_t*>(t[p1s_in_img_HI].get_dataptr());
        uint32_t* m_out_labels = static_cast<uint32_t*>(t[p1s_out_labels].get_dataptr());

        tools_linear_2d_nrc_ui32matrix(m_in_labels, mrg.i0 - mrg.b, mrg.i1 + mrg.b, mrg.j0 - mrg.b, mrg.j1 + mrg.b,
                                       mrg.in_labels);
        tools_linear_2d_nrc_ui8matrix(m_in_img_HI, mrg.i0 - mrg.b, mrg.i1 + mrg.b, mrg.j0 - mrg.b, mrg.j1 + mrg.b,
                                      mrg.in_img_HI);
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_out_labels, mrg.i0 - mrg.b, mrg.i1 + mrg.b, mrg.j0 - mrg.b,
                                      mrg.j1 + mrg.b, (const uint32_t**)mrg.out_labels);

        const uint32_t in_n_RoIs = *static_cast<const uint32_t*>(t[p1s_in_n_RoIs].get_dataptr());

        std::copy_n(static_cast<const uint32_t*>(t[p1s_in_RoIs_id].get_dataptr()), in_n_RoIs, mrg.tmp_in_RoIs_id);

        _features_merge_CCL_HI_v2(mrg.in_labels, mrg.in_img_HI, mrg.out_labels, mrg.i0, mrg.i1, mrg.j0, mrg.j1,
                                  mrg.tmp_in_RoIs_id,
                                  static_cast<const uint32_t*>(t[p1s_in_RoIs_xmin].get_dataptr()),
                                  static_cast<const uint32_t*>(t[p1s_in_RoIs_xmax].get_dataptr()),
                                  static_cast<const uint32_t*>(t[p1s_in_RoIs_ymin].get_dataptr()),
                                  static_cast<const uint32_t*>(t[p1s_in_RoIs_ymax].get_dataptr()),
                                  static_cast<const uint32_t*>(t[p1s_in_RoIs_S].get_dataptr()),
                                  in_n_RoIs,
                                  mrg.S_min, mrg.S_max);

        size_t out_n_RoIs = _features_shrink(mrg.tmp_in_RoIs_id,
                                             static_cast<const uint32_t*>(t[p1s_in_RoIs_xmin].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p1s_in_RoIs_xmax].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p1s_in_RoIs_ymin].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p1s_in_RoIs_ymax].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p1s_in_RoIs_S].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p1s_in_RoIs_Sx].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p1s_in_RoIs_Sy].get_dataptr()),
                                             static_cast<const float*>(t[p1s_in_RoIs_x].get_dataptr()),
                                             static_cast<const float*>(t[p1s_in_RoIs_y].get_dataptr()),
                                             in_n_RoIs,
                                             static_cast<uint32_t*>(t[p1s_out_RoIs_id].get_dataptr()),
                                             static_cast<uint32_t*>(t[p1s_out_RoIs_xmin].get_dataptr()),
                                             static_cast<uint32_t*>(t[p1s_out_RoIs_xmax].get_dataptr()),
                                             static_cast<uint32_t*>(t[p1s_out_RoIs_ymin].get_dataptr()),
                                             static_cast<uint32_t*>(t[p1s_out_RoIs_ymax].get_dataptr()),
                                             static_cast<uint32_t*>(t[p1s_out_RoIs_S].get_dataptr()),
                                             static_cast<uint32_t*>(t[p1s_out_RoIs_Sx].get_dataptr()),
                                             static_cast<uint32_t*>(t[p1s_out_RoIs_Sy].get_dataptr()),
                                             static_cast<float*>(t[p1s_out_RoIs_x].get_dataptr()),
                                             static_cast<float*>(t[p1s_out_RoIs_y].get_dataptr()));

        *static_cast<uint32_t*>(t[p1s_out_n_RoIs].get_dataptr()) = (uint32_t)out_n_RoIs;

        return aff3ct::runtime::status_t::SUCCESS;
    });


    auto &p2 = this->create_task("merge_v3");
    auto p2s_in_labels = this->template create_socket_in<uint32_t>(p2, "in_labels", socket_img_size);
    auto p2s_in_img = this->template create_socket_in<uint8_t>(p2, "in_img", socket_img_size);

    auto p2s_in_RoIs_id = this->template create_socket_in<uint32_t>(p2, "in_RoIs_id", max_in_RoIs_size);
    auto p2s_in_RoIs_xmin = this->template create_socket_in<uint32_t>(p2, "in_RoIs_xmin", max_in_RoIs_size);
    auto p2s_in_RoIs_xmax = this->template create_socket_in<uint32_t>(p2, "in_RoIs_xmax", max_in_RoIs_size);
    auto p2s_in_RoIs_ymin = this->template create_socket_in<uint32_t>(p2, "in_RoIs_ymin", max_in_RoIs_size);
    auto p2s_in_RoIs_ymax = this->template create_socket_in<uint32_t>(p2, "in_RoIs_ymax", max_in_RoIs_size);
    auto p2s_in_RoIs_S = this->template create_socket_in<uint32_t>(p2, "in_RoIs_S", max_in_RoIs_size);
    auto p2s_in_RoIs_Sx = this->template create_socket_in<uint32_t>(p2, "in_RoIs_Sx", max_in_RoIs_size);
    auto p2s_in_RoIs_Sy = this->template create_socket_in<uint32_t>(p2, "in_RoIs_Sy", max_in_RoIs_size);
    auto p2s_in_RoIs_x = this->template create_socket_in<float>(p2, "in_RoIs_x", max_in_RoIs_size);
    auto p2s_in_RoIs_y = this->template create_socket_in<float>(p2, "in_RoIs_y", max_in_RoIs_size);
    auto p2s_in_n_RoIs = this->template create_socket_in<uint32_t>(p2, "in_n_RoIs", 1);

    auto p2s_out_RoIs_id = this->template create_socket_out<uint32_t>(p2, "out_RoIs_id", max_out_RoIs_size);
    auto p2s_out_RoIs_xmin = this->template create_socket_out<uint32_t>(p2, "out_RoIs_xmin", max_out_RoIs_size);
    auto p2s_out_RoIs_xmax = this->template create_socket_out<uint32_t>(p2, "out_RoIs_xmax", max_out_RoIs_size);
    auto p2s_out_RoIs_ymin = this->template create_socket_out<uint32_t>(p2, "out_RoIs_ymin", max_out_RoIs_size);
    auto p2s_out_RoIs_ymax = this->template create_socket_out<uint32_t>(p2, "out_RoIs_ymax", max_out_RoIs_size);
    auto p2s_out_RoIs_S = this->template create_socket_out<uint32_t>(p2, "out_RoIs_S", max_out_RoIs_size);
    auto p2s_out_RoIs_Sx = this->template create_socket_out<uint32_t>(p2, "out_RoIs_Sx", max_out_RoIs_size);
    auto p2s_out_RoIs_Sy = this->template create_socket_out<uint32_t>(p2, "out_RoIs_Sy", max_out_RoIs_size);
    auto p2s_out_RoIs_x = this->template create_socket_out<float>(p2, "out_RoIs_x", max_out_RoIs_size);
    auto p2s_out_RoIs_y = this->template create_socket_out<float>(p2, "out_RoIs_y", max_out_RoIs_size);
    auto p2s_out_n_RoIs = this->template create_socket_out<uint32_t>(p2, "out_n_RoIs", 1);

    auto p2s_out_labels = this->template create_socket_out<uint32_t>(p2, "out_labels", socket_img_size);

    this->create_codelet(p2, [p2s_in_labels, p2s_in_img, p2s_in_RoIs_id, p2s_in_RoIs_xmin, p2s_in_RoIs_xmax,
                              p2s_in_RoIs_ymin, p2s_in_RoIs_ymax, p2s_in_RoIs_S, p2s_in_RoIs_Sx, p2s_in_RoIs_Sy,
                              p2s_in_RoIs_x, p2s_in_RoIs_y, p2s_in_n_RoIs, p2s_out_RoIs_id, p2s_out_RoIs_xmin,
                              p2s_out_RoIs_xmax, p2s_out_RoIs_ymin, p2s_out_RoIs_ymax, p2s_out_RoIs_S, p2s_out_RoIs_Sx,
                              p2s_out_RoIs_Sy, p2s_out_RoIs_x, p2s_out_RoIs_y, p2s_out_n_RoIs, p2s_out_labels]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &mrg = static_cast<Features_merger_CCL_HI&>(m);
        const uint32_t* m_in_labels = static_cast<const uint32_t*>(t[p2s_in_labels].get_dataptr());
        const uint8_t* m_in_img = static_cast<const uint8_t*>(t[p2s_in_img].get_dataptr());
        uint32_t* m_out_labels = static_cast<uint32_t*>(t[p2s_out_labels].get_dataptr());

        tools_linear_2d_nrc_ui32matrix(m_in_labels, mrg.i0 - mrg.b, mrg.i1 + mrg.b, mrg.j0 - mrg.b, mrg.j1 + mrg.b,
                                       mrg.in_labels);
        tools_linear_2d_nrc_ui8matrix(m_in_img, mrg.i0 - mrg.b, mrg.i1 + mrg.b, mrg.j0 - mrg.b, mrg.j1 + mrg.b,
                                      mrg.in_img_HI);
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_out_labels, mrg.i0 - mrg.b, mrg.i1 + mrg.b, mrg.j0 - mrg.b,
                                      mrg.j1 + mrg.b, (const uint32_t**)mrg.out_labels);

        const uint32_t in_n_RoIs = *static_cast<const uint32_t*>(t[p2s_in_n_RoIs].get_dataptr());

        std::copy_n(static_cast<const uint32_t*>(t[p2s_in_RoIs_id].get_dataptr()), in_n_RoIs, mrg.tmp_in_RoIs_id);

        _features_merge_CCL_HI_v3(mrg.in_labels, mrg.in_img_HI, mrg.out_labels, mrg.i0, mrg.i1, mrg.j0, mrg.j1,
                                  mrg.tmp_in_RoIs_id,
                                  static_cast<const uint32_t*>(t[p2s_in_RoIs_xmin].get_dataptr()),
                                  static_cast<const uint32_t*>(t[p2s_in_RoIs_xmax].get_dataptr()),
                                  static_cast<const uint32_t*>(t[p2s_in_RoIs_ymin].get_dataptr()),
                                  static_cast<const uint32_t*>(t[p2s_in_RoIs_ymax].get_dataptr()),
                                  static_cast<const uint32_t*>(t[p2s_in_RoIs_S].get_dataptr()),
                                  in_n_RoIs,
                                  mrg.S_min, mrg.S_max, mrg.threshold_high, mrg.no_labels_zeros_init);

        size_t out_n_RoIs = _features_shrink(mrg.tmp_in_RoIs_id,
                                             static_cast<const uint32_t*>(t[p2s_in_RoIs_xmin].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p2s_in_RoIs_xmax].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p2s_in_RoIs_ymin].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p2s_in_RoIs_ymax].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p2s_in_RoIs_S].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p2s_in_RoIs_Sx].get_dataptr()),
                                             static_cast<const uint32_t*>(t[p2s_in_RoIs_Sy].get_dataptr()),
                                             static_cast<const float*>(t[p2s_in_RoIs_x].get_dataptr()),
                                             static_cast<const float*>(t[p2s_in_RoIs_y].get_dataptr()),
                                             in_n_RoIs,
                                             static_cast<uint32_t*>(t[p2s_out_RoIs_id].get_dataptr()),
                                             static_cast<uint32_t*>(t[p2s_out_RoIs_xmin].get_dataptr()),
                                             static_cast<uint32_t*>(t[p2s_out_RoIs_xmax].get_dataptr()),
                                             static_cast<uint32_t*>(t[p2s_out_RoIs_ymin].get_dataptr()),
                                             static_cast<uint32_t*>(t[p2s_out_RoIs_ymax].get_dataptr()),
                                             static_cast<uint32_t*>(t[p2s_out_RoIs_S].get_dataptr()),
                                             static_cast<uint32_t*>(t[p2s_out_RoIs_Sx].get_dataptr()),
                                             static_cast<uint32_t*>(t[p2s_out_RoIs_Sy].get_dataptr()),
                                             static_cast<float*>(t[p2s_out_RoIs_x].get_dataptr()),
                                             static_cast<float*>(t[p2s_out_RoIs_y].get_dataptr()));

        *static_cast<uint32_t*>(t[p2s_out_n_RoIs].get_dataptr()) = (uint32_t)out_n_RoIs;

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

void Features_merger_CCL_HI::init_data() {
    this->tmp_in_RoIs_id = (uint32_t*)malloc(this->max_in_RoIs_size * sizeof(uint32_t));
    this->in_labels = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint32_t*)));
    this->in_img_HI = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint8_t*)));
    this->out_labels = (uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(uint32_t*)));
    this->in_labels -= i0 - b;
    this->in_img_HI -= i0 - b;
    this->out_labels -= i0 - b;
}

Features_merger_CCL_HI::~Features_merger_CCL_HI() {
    free(this->tmp_in_RoIs_id);
    free(this->in_labels + (this->i0 - this->b));
    free(this->in_img_HI + (this->i0 - this->b));
    free(this->out_labels + (this->i0 - this->b));
}

Features_merger_CCL_HI* Features_merger_CCL_HI::clone() const {
    auto m = new Features_merger_CCL_HI(*this);
    m->deep_copy(*this);
    return m;
}

void Features_merger_CCL_HI::deep_copy(const Features_merger_CCL_HI &m)
{
    Module::deep_copy(m);
    this->init_data();
}
