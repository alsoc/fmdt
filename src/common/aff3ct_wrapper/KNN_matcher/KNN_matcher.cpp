#include "fmdt/KNN/KNN_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/KNN_matcher/KNN_matcher.hpp"

KNN_matcher::KNN_matcher(const size_t k, const uint32_t max_dist, const float min_ratio_S, const size_t max_size)
: Module(), k(k), max_dist(max_dist), min_ratio_S(min_ratio_S), max_size(max_size), /* data(nullptr), */
  out_data_nearest(nullptr), out_data_distances(nullptr) {
    const std::string name = "KNN_matcher";
    this->set_name(name);
    this->set_short_name(name);

    this->out_data_nearest = (uint32_t**)malloc((size_t)(max_size * sizeof(const uint32_t*)));
    this->out_data_distances = (float**)malloc((size_t)(max_size * sizeof(float*)));

    auto &p = this->create_task("match");

    auto ps_in_ROI0_id = this->template create_socket_in<uint32_t>(p, "in_ROI0_id", max_size);
    auto ps_in_ROI0_S = this->template create_socket_in<uint32_t>(p, "in_ROI0_S", max_size);
    auto ps_in_ROI0_x = this->template create_socket_in<float>(p, "in_ROI0_x", max_size);
    auto ps_in_ROI0_y = this->template create_socket_in<float>(p, "in_ROI0_y", max_size);
    auto ps_in_n_ROI0 = this->template create_socket_in<uint32_t>(p, "in_n_ROI0", 1);
    auto ps_in_ROI1_id = this->template create_socket_in<uint32_t>(p, "in_ROI1_id", max_size);
    auto ps_in_ROI1_S = this->template create_socket_in<uint32_t>(p, "in_ROI1_S", max_size);
    auto ps_in_ROI1_x = this->template create_socket_in<float>(p, "in_ROI1_x", max_size);
    auto ps_in_ROI1_y = this->template create_socket_in<float>(p, "in_ROI1_y", max_size);
    auto ps_in_n_ROI1 = this->template create_socket_in<uint32_t>(p, "in_n_ROI1", 1);

    auto ps_out_ROI0_next_id = this->template create_socket_out<uint32_t>(p, "out_ROI0_next_id", max_size);
    auto ps_out_ROI1_prev_id = this->template create_socket_out<uint32_t>(p, "out_ROI1_prev_id", max_size);
    auto ps_out_data_nearest = this->template create_socket_out<uint32_t>(p, "out_data_nearest", max_size * max_size);
    auto ps_out_data_distances = this->template create_socket_out<float>(p, "out_data_distances", max_size * max_size);
    auto ps_out_data_conflicts = this->template create_socket_out<uint32_t>(p, "out_data_conflicts", max_size);

    this->create_codelet(p, [ps_in_ROI0_id, ps_in_ROI0_S, ps_in_ROI0_x, ps_in_ROI0_y, ps_in_n_ROI0, ps_in_ROI1_id,
                             ps_in_ROI1_S, ps_in_ROI1_x, ps_in_ROI1_y, ps_in_n_ROI1, ps_out_ROI0_next_id,
                             ps_out_ROI1_prev_id, ps_out_data_nearest, ps_out_data_distances, ps_out_data_conflicts]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &knn = static_cast<KNN_matcher&>(m);

        const size_t n_ROI0 = (size_t)*static_cast<const uint32_t*>(t[ps_in_n_ROI0].get_dataptr());
        const size_t n_ROI1 = (size_t)*static_cast<const uint32_t*>(t[ps_in_n_ROI1].get_dataptr());

        uint32_t* m_out_data_nearest = static_cast<uint32_t*>(t[ps_out_data_nearest].get_dataptr());
        float* m_out_data_distances = static_cast<float*>(t[ps_out_data_distances].get_dataptr());
        
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_out_data_nearest, 0, knn.max_size -1, 0, knn.max_size -1,
                                       (const uint32_t**)knn.out_data_nearest);        
        tools_linear_2d_nrc_f32matrix((const float*)m_out_data_distances, 0, knn.max_size -1, 0, knn.max_size -1,
                                       (const float**)knn.out_data_distances);     

        _KNN_match(knn.out_data_nearest,
                   knn.out_data_distances,
                   static_cast<uint32_t*>(t[ps_out_data_conflicts].get_dataptr()),
                   static_cast<const uint32_t*>(t[ps_in_ROI0_id].get_dataptr()),
                   static_cast<const uint32_t*>(t[ps_in_ROI0_S].get_dataptr()),
                   static_cast<const float*>(t[ps_in_ROI0_x].get_dataptr()),
                   static_cast<const float*>(t[ps_in_ROI0_y].get_dataptr()),
                   static_cast<uint32_t*>(t[ps_out_ROI0_next_id].get_dataptr()),
                   n_ROI0,
                   static_cast<const uint32_t*>(t[ps_in_ROI1_id].get_dataptr()),
                   static_cast<const uint32_t*>(t[ps_in_ROI1_S].get_dataptr()),
                   static_cast<const float*>(t[ps_in_ROI1_x].get_dataptr()),
                   static_cast<const float*>(t[ps_in_ROI1_y].get_dataptr()),
                   static_cast<uint32_t*>(t[ps_out_ROI1_prev_id].get_dataptr()),
                   n_ROI1, knn.k, knn.max_dist, knn.min_ratio_S);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

KNN_matcher::~KNN_matcher() {
    free(this->out_data_nearest);
    free(this->out_data_distances);
}
