#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

#include "fmdt/KPPV.h"

namespace knn {
    enum class tsk : size_t { match, SIZE };
    namespace sck {
        enum class match : size_t { in_ROI0_id, in_ROI0_x, in_ROI0_y, in_n_ROI0, in_ROI1_id, in_ROI1_x, in_ROI1_y,
                                    in_n_ROI1, out_ROI0_next_id, out_ROI1_prev_id, out_data_nearest, out_data_distances,
                                    out_data_conflicts, status };
    }
}

class KNN_matcher : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const size_t k;
    uint32_t** out_data_nearest;
    float** out_data_distances;
public:
    KNN_matcher(const size_t i0, const int i1, const int j0, const int j1, const size_t k, const size_t max_ROI_size)
    : Module(), i0(i0), i1(i1), j0(j0), j1(j1), k(k), /* data(nullptr), */ out_data_nearest(nullptr),
      out_data_distances(nullptr)  {
        const std::string name = "KNN_matcher";
        this->set_name(name);
        this->set_short_name(name);

        this->out_data_nearest = (uint32_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(const uint32_t*)));
        this->out_data_nearest -= i0;
        this->out_data_distances = (float**)malloc((size_t)(((i1 - i0) + 1) * sizeof(float*)));
        this->out_data_distances -= i0;

        auto &p = this->create_task("match");

        auto ps_in_ROI0_id = this->template create_socket_in<uint16_t>(p, "in_ROI0_id", max_ROI_size);
        auto ps_in_ROI0_x = this->template create_socket_in<float>(p, "in_ROI0_x", max_ROI_size);
        auto ps_in_ROI0_y = this->template create_socket_in<float>(p, "in_ROI0_y", max_ROI_size);
        auto ps_in_n_ROI0 = this->template create_socket_in<uint32_t>(p, "in_n_ROI0", 1);
        auto ps_in_ROI1_id = this->template create_socket_in<uint16_t>(p, "in_ROI1_id", max_ROI_size);
        auto ps_in_ROI1_x = this->template create_socket_in<float>(p, "in_ROI1_x", max_ROI_size);
        auto ps_in_ROI1_y = this->template create_socket_in<float>(p, "in_ROI1_y", max_ROI_size);
        auto ps_in_n_ROI1 = this->template create_socket_in<uint32_t>(p, "in_n_ROI1", 1);

        const size_t d_socket_size = ((i1 - i0) + 1) * ((j1 - j0) + 1);
        auto ps_out_ROI0_next_id = this->template create_socket_out<int32_t>(p, "out_ROI0_next_id", max_ROI_size);
        auto ps_out_ROI1_prev_id = this->template create_socket_out<int32_t>(p, "out_ROI1_prev_id", max_ROI_size);
        auto ps_out_data_nearest = this->template create_socket_out<uint32_t>(p, "out_data_nearest", d_socket_size);
        auto ps_out_data_distances = this->template create_socket_out<float>(p, "out_data_distances", d_socket_size);
        auto ps_out_data_conflicts = this->template create_socket_out<uint32_t>(p, "out_data_conflicts", j1 - j0 + 1);

        this->create_codelet(p, [ps_in_ROI0_id, ps_in_ROI0_x, ps_in_ROI0_y, ps_in_n_ROI0, ps_in_ROI1_id, ps_in_ROI1_x,
                                 ps_in_ROI1_y, ps_in_n_ROI1, ps_out_ROI0_next_id, ps_out_ROI1_prev_id,
                                 ps_out_data_nearest, ps_out_data_distances, ps_out_data_conflicts]
                             (aff3ct::module::Module &m, aff3ct::module::Task &t, const size_t frame_id) -> int {
            auto &knn = static_cast<KNN_matcher&>(m);

            const size_t n_ROI0 = (size_t)*static_cast<const uint32_t*>(t[ps_in_n_ROI0].get_dataptr());
            const size_t n_ROI1 = (size_t)*static_cast<const uint32_t*>(t[ps_in_n_ROI1].get_dataptr());

            uint32_t* m_out_data_nearest = static_cast<uint32_t*>(t[ps_out_data_nearest].get_dataptr());
            float* m_out_data_distances = static_cast<float*>(t[ps_out_data_distances].get_dataptr());
            knn.out_data_nearest[knn.i0] = m_out_data_nearest - knn.j0;
            knn.out_data_distances[knn.i0] = m_out_data_distances - knn.j0;
            for (int i = knn.i0 + 1; i <= knn.i1; i++) {
                knn.out_data_nearest[i] = knn.out_data_nearest[i - 1] + ((knn.j1 - knn.j0) + 1);
                knn.out_data_distances[i] = knn.out_data_distances[i - 1] + ((knn.j1 - knn.j0) + 1);
            }

            std::fill_n(static_cast<int32_t*>(t[ps_out_ROI0_next_id].get_dataptr()), n_ROI0, 0);
            std::fill_n(static_cast<int32_t*>(t[ps_out_ROI1_prev_id].get_dataptr()), n_ROI1, 0);
            _KPPV_match(knn.out_data_nearest,
                        knn.out_data_distances,
                        static_cast<uint32_t*>(t[ps_out_data_conflicts].get_dataptr()),
                        static_cast<const uint16_t*>(t[ps_in_ROI0_id].get_dataptr()),
                        static_cast<const float*>(t[ps_in_ROI0_x].get_dataptr()),
                        static_cast<const float*>(t[ps_in_ROI0_y].get_dataptr()),
                        static_cast<int32_t*>(t[ps_out_ROI0_next_id].get_dataptr()),
                        n_ROI0,
                        static_cast<const uint16_t*>(t[ps_in_ROI1_id].get_dataptr()),
                        static_cast<const float*>(t[ps_in_ROI1_x].get_dataptr()),
                        static_cast<const float*>(t[ps_in_ROI1_y].get_dataptr()),
                        static_cast<int32_t*>(t[ps_out_ROI1_prev_id].get_dataptr()),
                        n_ROI1, knn.k);

            return aff3ct::module::status_t::SUCCESS;
        });
    }

    virtual ~KNN_matcher() {
        free(this->out_data_nearest + this->i0);
        free(this->out_data_distances + this->i0);
    }

    inline aff3ct::module::Task& operator[](const knn::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const knn::sck::match s) {
        return aff3ct::module::Module::operator[]((size_t)knn::tsk::match)[(size_t)s];
    }
};
