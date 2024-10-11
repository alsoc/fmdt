#include "fmdt/kNN/kNN_compute.h"
#include "fmdt/tools.h"

#include "fmdt/spu/kNN_matcher/kNN_matcher.hpp"

kNN_matcher::kNN_matcher(const size_t k, const uint32_t max_dist, const float min_ratio_S, const size_t max_size)
: spu::module::Stateful(), k(k), max_dist(max_dist), min_ratio_S(min_ratio_S), max_size(max_size) {
    const std::string name = "kNN_matcher";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("match");

    auto ps_in_RoIs0_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs0_basic",
                                                                      max_size * sizeof(RoI_basic_t));
    auto ps_in_n_RoIs0 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs0", 1);
    auto ps_in_RoIs1_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs1_basic",
                                                                      max_size * sizeof(RoI_basic_t));
    auto ps_in_n_RoIs1 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs1", 1);

    auto ps_out_RoIs0_asso = this->template create_socket_out<uint8_t>(p, "out_RoIs0_asso",
                                                                       max_size * sizeof(RoI_asso_t));
    auto ps_out_RoIs1_asso = this->template create_socket_out<uint8_t>(p, "out_RoIs1_asso",
                                                                       max_size * sizeof(RoI_asso_t));
    auto ps_out_data_distances = this->template create_2d_socket_out<float>(p, "out_data_distances", max_size, max_size);
    auto ps_out_data_nearest = this->template create_2d_socket_out<uint32_t>(p, "out_data_nearest", max_size, max_size);
    auto ps_out_data_conflicts = this->template create_socket_out<uint32_t>(p, "out_data_conflicts", max_size);

    this->create_codelet(p, [ps_in_RoIs0_basic, ps_in_n_RoIs0, ps_in_RoIs1_basic, ps_in_n_RoIs1, ps_out_RoIs0_asso,
                             ps_out_RoIs1_asso, ps_out_data_nearest, ps_out_data_distances, ps_out_data_conflicts]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
        auto &knn = static_cast<kNN_matcher&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
              float**      out_data_distances =  t[ps_out_data_distances].get_2d_dataptr<      float      >();
              uint32_t**   out_data_nearest   =  t[ps_out_data_nearest  ].get_2d_dataptr<      uint32_t   >();
              uint32_t*    out_data_conflicts =  t[ps_out_data_conflicts].get_dataptr   <      uint32_t   >();
        const RoI_basic_t* in_RoIs0_basic     =  t[ps_in_RoIs0_basic    ].get_dataptr   <const RoI_basic_t>();
              RoI_asso_t*  out_RoIs0_asso     =  t[ps_out_RoIs0_asso    ].get_dataptr   <      RoI_asso_t >();
        const uint32_t     in_n_RoIs0         = *t[ps_in_n_RoIs0        ].get_dataptr   <const uint32_t   >();
        const RoI_basic_t* in_RoIs1_basic     =  t[ps_in_RoIs1_basic    ].get_dataptr   <const RoI_basic_t>();
              RoI_asso_t*  out_RoIs1_asso     =  t[ps_out_RoIs1_asso    ].get_dataptr   <      RoI_asso_t >();
        const uint32_t     in_n_RoIs1         = *t[ps_in_n_RoIs1        ].get_dataptr   <const uint32_t   >();

        kNN_data_t kNN_data = { out_data_distances, out_data_nearest, out_data_conflicts, knn.max_size };

        kNN_match(&kNN_data, in_RoIs0_basic, out_RoIs0_asso, in_n_RoIs0, in_RoIs1_basic, out_RoIs1_asso, in_n_RoIs1,
                  knn.k, knn.max_dist, knn.min_ratio_S);

        return spu::runtime::status_t::SUCCESS;
    });
}

kNN_matcher::~kNN_matcher() {
}

kNN_matcher* kNN_matcher::clone() const {
    auto m = new kNN_matcher(*this);
    m->deep_copy(*this);
    return m;
}
