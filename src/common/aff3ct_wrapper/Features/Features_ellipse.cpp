#include "fmdt/features/features_compute.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Features/Features_ellipse.hpp"

Features_ellipse::Features_ellipse(const size_t max_RoIs_size)
: Module(), max_RoIs_size(max_RoIs_size) {
    const std::string name = "Features_ellipse";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("compute");

    auto ps_in_RoIs_S = this->template create_socket_in<uint32_t>(p, "in_RoIs_S", max_RoIs_size);
    auto ps_in_RoIs_Sx = this->template create_socket_in<uint32_t>(p, "in_RoIs_Sx", max_RoIs_size);
    auto ps_in_RoIs_Sy = this->template create_socket_in<uint32_t>(p, "in_RoIs_Sy", max_RoIs_size);
    auto ps_in_RoIs_Sx2 = this->template create_socket_in<uint64_t>(p, "in_RoIs_Sx2", max_RoIs_size);
    auto ps_in_RoIs_Sy2 = this->template create_socket_in<uint64_t>(p, "in_RoIs_Sy2", max_RoIs_size);
    auto ps_in_RoIs_Sxy = this->template create_socket_in<uint64_t>(p, "in_RoIs_Sxy", max_RoIs_size);
    auto ps_in_n_RoIs = this->template create_socket_in<uint32_t>(p, "in_n_RoIs", 1);
    auto ps_out_RoIs_a = this->template create_socket_out<float>(p, "out_RoIs_a", max_RoIs_size);
    auto ps_out_RoIs_b = this->template create_socket_out<float>(p, "out_RoIs_b", max_RoIs_size);

    this->create_codelet(p, [ps_in_RoIs_S, ps_in_RoIs_Sx, ps_in_RoIs_Sy, ps_in_RoIs_Sx2, ps_in_RoIs_Sy2, ps_in_RoIs_Sxy,
                             ps_in_n_RoIs, ps_out_RoIs_a, ps_out_RoIs_b]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        // auto &ell = static_cast<Features_ellipse&>(m);

        const uint32_t in_n_RoIs = *static_cast<const uint32_t*>(t[ps_in_n_RoIs].get_dataptr());

        _features_compute_ellipse(static_cast<const uint32_t*>(t[ps_in_RoIs_S].get_dataptr()),
                                  static_cast<const uint32_t*>(t[ps_in_RoIs_Sx].get_dataptr()),
                                  static_cast<const uint32_t*>(t[ps_in_RoIs_Sy].get_dataptr()),
                                  static_cast<const uint64_t*>(t[ps_in_RoIs_Sx2].get_dataptr()),
                                  static_cast<const uint64_t*>(t[ps_in_RoIs_Sy2].get_dataptr()),
                                  static_cast<const uint64_t*>(t[ps_in_RoIs_Sxy].get_dataptr()),
                                  static_cast<float*>(t[ps_out_RoIs_a].get_dataptr()),
                                  static_cast<float*>(t[ps_out_RoIs_b].get_dataptr()),
                                  in_n_RoIs);

        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Features_ellipse* Features_ellipse::clone() const {
    auto m = new Features_ellipse(*this);
    m->deep_copy(*this);
    return m;
}
