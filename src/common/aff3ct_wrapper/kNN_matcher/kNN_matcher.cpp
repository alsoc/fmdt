#include "fmdt/kNN/kNN_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/kNN_matcher/kNN_matcher.hpp"

kNN_matcher::kNN_matcher(const size_t k, const uint32_t max_dist, const float min_ratio_S, const size_t max_size)
: Module(), k(k), max_dist(max_dist), min_ratio_S(min_ratio_S), max_size(max_size), /* data(nullptr), */
  out_data_nearest(nullptr), out_data_distances(nullptr) {
    const std::string name = "kNN_matcher";
    this->set_name(name);
    this->set_short_name(name);

    this->init_data();

    auto &p = this->create_task("match");

    auto ps_in_RoIs0_id = this->template create_socket_in<uint32_t>(p, "in_RoIs0_id", max_size);
    auto ps_in_RoIs0_S = this->template create_socket_in<uint32_t>(p, "in_RoIs0_S", max_size);
    auto ps_in_RoIs0_x = this->template create_socket_in<float>(p, "in_RoIs0_x", max_size);
    auto ps_in_RoIs0_y = this->template create_socket_in<float>(p, "in_RoIs0_y", max_size);
    auto ps_in_n_RoIs0 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs0", 1);
    auto ps_in_RoIs1_id = this->template create_socket_in<uint32_t>(p, "in_RoIs1_id", max_size);
    auto ps_in_RoIs1_S = this->template create_socket_in<uint32_t>(p, "in_RoIs1_S", max_size);
    auto ps_in_RoIs1_x = this->template create_socket_in<float>(p, "in_RoIs1_x", max_size);
    auto ps_in_RoIs1_y = this->template create_socket_in<float>(p, "in_RoIs1_y", max_size);
    auto ps_in_n_RoIs1 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs1", 1);

    auto ps_out_RoIs0_next_id = this->template create_socket_out<uint32_t>(p, "out_RoIs0_next_id", max_size);
    auto ps_out_RoIs1_prev_id = this->template create_socket_out<uint32_t>(p, "out_RoIs1_prev_id", max_size);
    auto ps_out_data_nearest = this->template create_socket_out<uint32_t>(p, "out_data_nearest", max_size * max_size);
    auto ps_out_data_distances = this->template create_socket_out<float>(p, "out_data_distances", max_size * max_size);
    auto ps_out_data_conflicts = this->template create_socket_out<uint32_t>(p, "out_data_conflicts", max_size);

    this->create_codelet(p, [ps_in_RoIs0_id, ps_in_RoIs0_S, ps_in_RoIs0_x, ps_in_RoIs0_y, ps_in_n_RoIs0, ps_in_RoIs1_id,
                             ps_in_RoIs1_S, ps_in_RoIs1_x, ps_in_RoIs1_y, ps_in_n_RoIs1, ps_out_RoIs0_next_id,
                             ps_out_RoIs1_prev_id, ps_out_data_nearest, ps_out_data_distances, ps_out_data_conflicts]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &knn = static_cast<kNN_matcher&>(m);

        const size_t n_RoIs0 = (size_t)*static_cast<const uint32_t*>(t[ps_in_n_RoIs0].get_dataptr());
        const size_t n_RoIs1 = (size_t)*static_cast<const uint32_t*>(t[ps_in_n_RoIs1].get_dataptr());

        uint32_t* m_out_data_nearest = static_cast<uint32_t*>(t[ps_out_data_nearest].get_dataptr());
        float* m_out_data_distances = static_cast<float*>(t[ps_out_data_distances].get_dataptr());
        
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_out_data_nearest, 0, knn.max_size -1, 0, knn.max_size -1,
                                       (const uint32_t**)knn.out_data_nearest);        
        tools_linear_2d_nrc_f32matrix((const float*)m_out_data_distances, 0, knn.max_size -1, 0, knn.max_size -1,
                                       (const float**)knn.out_data_distances);     

        _kNN_match(knn.out_data_distances,
                   knn.out_data_nearest,
                   static_cast<uint32_t*>(t[ps_out_data_conflicts].get_dataptr()),
                   static_cast<const uint32_t*>(t[ps_in_RoIs0_id].get_dataptr()),
                   static_cast<const uint32_t*>(t[ps_in_RoIs0_S].get_dataptr()),
                   static_cast<const float*>(t[ps_in_RoIs0_x].get_dataptr()),
                   static_cast<const float*>(t[ps_in_RoIs0_y].get_dataptr()),
                   static_cast<uint32_t*>(t[ps_out_RoIs0_next_id].get_dataptr()),
                   n_RoIs0,
                   static_cast<const uint32_t*>(t[ps_in_RoIs1_id].get_dataptr()),
                   static_cast<const uint32_t*>(t[ps_in_RoIs1_S].get_dataptr()),
                   static_cast<const float*>(t[ps_in_RoIs1_x].get_dataptr()),
                   static_cast<const float*>(t[ps_in_RoIs1_y].get_dataptr()),
                   static_cast<uint32_t*>(t[ps_out_RoIs1_prev_id].get_dataptr()),
                   n_RoIs1, knn.k, knn.max_dist, knn.min_ratio_S);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

void kNN_matcher::init_data() {
    this->out_data_nearest = (uint32_t**)malloc((size_t)(this->max_size * sizeof(const uint32_t*)));
    this->out_data_distances = (float**)malloc((size_t)(this->max_size * sizeof(float*)));
}

kNN_matcher::~kNN_matcher() {
    free(this->out_data_nearest);
    free(this->out_data_distances);
}

kNN_matcher* kNN_matcher::clone() const {
    auto m = new kNN_matcher(*this);
    m->deep_copy(*this);
    return m;
}

void kNN_matcher::deep_copy(const kNN_matcher &m)
{
    Module::deep_copy(m);
    this->init_data();
}
