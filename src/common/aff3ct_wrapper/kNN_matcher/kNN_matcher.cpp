#include "fmdt/kNN/kNN_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/kNN_matcher/kNN_matcher.hpp"

kNN_matcher::kNN_matcher(const size_t k, const uint32_t max_dist, const float min_ratio_S, const size_t max_size)
: Module(), k(k), max_dist(max_dist), min_ratio_S(min_ratio_S), max_size(max_size) {
    const std::string name = "kNN_matcher";
    this->set_name(name);
    this->set_short_name(name);

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
    auto ps_out_data_distances = this->template create_2d_socket_out<float>(p, "out_data_distances", max_size, max_size);
    auto ps_out_data_nearest = this->template create_2d_socket_out<uint32_t>(p, "out_data_nearest", max_size, max_size);
    auto ps_out_data_conflicts = this->template create_socket_out<uint32_t>(p, "out_data_conflicts", max_size);

    this->create_codelet(p, [ps_in_RoIs0_id, ps_in_RoIs0_S, ps_in_RoIs0_x, ps_in_RoIs0_y, ps_in_n_RoIs0, ps_in_RoIs1_id,
                             ps_in_RoIs1_S, ps_in_RoIs1_x, ps_in_RoIs1_y, ps_in_n_RoIs1, ps_out_RoIs0_next_id,
                             ps_out_RoIs1_prev_id, ps_out_data_nearest, ps_out_data_distances, ps_out_data_conflicts]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &knn = static_cast<kNN_matcher&>(m);

        const size_t n_RoIs0 = (size_t)*static_cast<const uint32_t*>(t[ps_in_n_RoIs0].get_dataptr());
        const size_t n_RoIs1 = (size_t)*static_cast<const uint32_t*>(t[ps_in_n_RoIs1].get_dataptr());

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        float** out_data_distances = t[ps_out_data_distances].get_2d_dataptr<float>();
        uint32_t** out_data_nearest = t[ps_out_data_nearest].get_2d_dataptr<uint32_t>();

        _kNN_match(out_data_distances,
                   out_data_nearest,
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

kNN_matcher::~kNN_matcher() {
}

kNN_matcher* kNN_matcher::clone() const {
    auto m = new kNN_matcher(*this);
    m->deep_copy(*this);
    return m;
}
