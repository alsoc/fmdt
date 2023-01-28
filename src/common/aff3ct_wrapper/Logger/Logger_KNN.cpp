#include "fmdt/KNN/KNN_io.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Logger/Logger_KNN.hpp"

Logger_KNN::Logger_KNN(const std::string KNN_path, const size_t fra_start, const size_t max_size)
: Module(), KNN_path(KNN_path), fra_start(fra_start), max_size(max_size) {
    const std::string name = "Logger_KNN";
    this->set_name(name);
    this->set_short_name(name);

    this->in_data_nearest = (const uint32_t**)malloc((size_t)(max_size * sizeof(uint32_t*)));
    this->in_data_distances = (const float**)malloc((size_t)(max_size * sizeof(float*)));

    auto &p = this->create_task("write");

    auto ps_in_data_nearest = this->template create_socket_in<uint32_t>(p, "in_data_nearest", max_size * max_size);
    auto ps_in_data_distances = this->template create_socket_in<float>(p, "in_data_distances", max_size * max_size);
    auto ps_in_data_conflicts = this->template create_socket_in<uint32_t>(p, "in_data_conflicts", max_size);

    auto ps_in_ROI0_id = this->template create_socket_in<uint32_t>(p, "in_ROI0_id", max_size);
    auto ps_in_ROI0_next_id = this->template create_socket_in<uint32_t>(p, "in_ROI0_next_id", max_size);
    auto ps_in_n_ROI0 = this->template create_socket_in<uint32_t>(p, "in_n_ROI0", 1);
    auto ps_in_ROI1_dx = this->template create_socket_in<float>(p, "in_ROI1_dx", max_size);
    auto ps_in_ROI1_dy = this->template create_socket_in<float>(p, "in_ROI1_dy", max_size);
    auto ps_in_ROI1_error = this->template create_socket_in<float>(p, "in_ROI1_error", max_size);
    auto ps_in_ROI1_is_moving = this->template create_socket_in<uint8_t>(p, "in_ROI1_is_moving", max_size);
    auto ps_in_n_ROI1 = this->template create_socket_in<uint32_t>(p, "in_n_ROI1", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!KNN_path.empty())
        tools_create_folder(KNN_path.c_str());

    this->create_codelet(p, [ps_in_data_nearest, ps_in_data_distances, ps_in_data_conflicts, ps_in_ROI0_id,
                             ps_in_ROI0_next_id, ps_in_n_ROI0, ps_in_ROI1_dx, ps_in_ROI1_dy, ps_in_ROI1_error,
                             ps_in_ROI1_is_moving, ps_in_n_ROI1, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_knn = static_cast<Logger_KNN&>(m);

        const uint32_t* m_in_data_nearest = static_cast<const uint32_t*>(t[ps_in_data_nearest].get_dataptr());
        const float* m_in_data_distances = static_cast<const float*>(t[ps_in_data_distances].get_dataptr());
        tools_linear_2d_nrc_ui32matrix((const uint32_t*)m_in_data_nearest, 0, lgr_knn.max_size -1, 0,
                                       lgr_knn.max_size -1, (const uint32_t**)lgr_knn.in_data_nearest);
        tools_linear_2d_nrc_f32matrix((const float*)m_in_data_distances, 0, lgr_knn.max_size - 1, 0,
                                      lgr_knn.max_size -1, (const float**)lgr_knn.in_data_distances);

        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        if (frame > (uint32_t)lgr_knn.fra_start && !lgr_knn.KNN_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_knn.KNN_path.c_str(), frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            _KNN_asso_conflicts_write(file, lgr_knn.in_data_nearest, lgr_knn.in_data_distances,
                                      static_cast<const uint32_t*>(t[ps_in_data_conflicts].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_id].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_ROI0_next_id].get_dataptr()),
                                      *static_cast<const uint32_t*>(t[ps_in_n_ROI0].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI1_dx].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI1_dy].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_ROI1_error].get_dataptr()),
                                      static_cast<const uint8_t*>(t[ps_in_ROI1_is_moving].get_dataptr()),
                                      *static_cast<const uint32_t*>(t[ps_in_n_ROI1].get_dataptr()));
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_KNN::~Logger_KNN() {
    free(this->in_data_nearest);
    free(this->in_data_distances);
}
