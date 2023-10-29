#include "fmdt/kNN/kNN_io.h"
#include "fmdt/tools.h"

#include "fmdt/aff3ct_wrapper/Logger/Logger_kNN.hpp"

Logger_kNN::Logger_kNN(const std::string kNN_path, const size_t fra_start, const size_t max_size)
: Module(), kNN_path(kNN_path), fra_start(fra_start), max_size(max_size) {
    const std::string name = "Logger_kNN";
    this->set_name(name);
    this->set_short_name(name);

    auto &p = this->create_task("write");

    auto ps_in_data_nearest = this->template create_2d_socket_in<uint32_t>(p, "in_data_nearest", max_size, max_size);
    auto ps_in_data_distances = this->template create_2d_socket_in<float>(p, "in_data_distances", max_size, max_size);
#ifdef FMDT_ENABLE_DEBUG
    auto ps_in_data_conflicts = this->template create_socket_in<uint32_t>(p, "in_data_conflicts", max_size);
#else
    this->template create_socket_in<uint32_t>(p, "in_data_conflicts", max_size);
#endif
    auto ps_in_RoIs0_id = this->template create_socket_in<uint32_t>(p, "in_RoIs0_id", max_size);
    auto ps_in_RoIs0_next_id = this->template create_socket_in<uint32_t>(p, "in_RoIs0_next_id", max_size);
    auto ps_in_n_RoIs0 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs0", 1);
    auto ps_in_RoIs1_dx = this->template create_socket_in<float>(p, "in_RoIs1_dx", max_size);
    auto ps_in_RoIs1_dy = this->template create_socket_in<float>(p, "in_RoIs1_dy", max_size);
    auto ps_in_RoIs1_error = this->template create_socket_in<float>(p, "in_RoIs1_error", max_size);
    auto ps_in_RoIs1_is_moving = this->template create_socket_in<uint8_t>(p, "in_RoIs1_is_moving", max_size);
    auto ps_in_n_RoIs1 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs1", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!kNN_path.empty())
        tools_create_folder(kNN_path.c_str());

#ifdef FMDT_ENABLE_DEBUG
    this->create_codelet(p, [ps_in_data_nearest, ps_in_data_distances, ps_in_data_conflicts, ps_in_RoIs0_id,
                             ps_in_RoIs0_next_id, ps_in_n_RoIs0, ps_in_RoIs1_dx, ps_in_RoIs1_dy, ps_in_RoIs1_error,
                             ps_in_RoIs1_is_moving, ps_in_n_RoIs1, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
#else
    this->create_codelet(p, [ps_in_data_nearest, ps_in_data_distances, ps_in_RoIs0_id, ps_in_RoIs0_next_id,
                             ps_in_n_RoIs0, ps_in_RoIs1_dx, ps_in_RoIs1_dy, ps_in_RoIs1_error, ps_in_RoIs1_is_moving,
                             ps_in_n_RoIs1, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
#endif
        auto &lgr_knn = static_cast<Logger_kNN&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const float** in_data_distances = t[ps_in_data_distances].get_2d_dataptr<const float>();
        const uint32_t** in_data_nearest = t[ps_in_data_nearest].get_2d_dataptr<const uint32_t>();

        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        if (frame > (uint32_t)lgr_knn.fra_start && !lgr_knn.kNN_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_knn.kNN_path.c_str(), frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            _kNN_asso_conflicts_write(file, in_data_distances, in_data_nearest,
#ifdef FMDT_ENABLE_DEBUG
                                      static_cast<const uint32_t*>(t[ps_in_data_conflicts].get_dataptr()),
#else
                                      nullptr,
#endif
                                      static_cast<const uint32_t*>(t[ps_in_RoIs0_id].get_dataptr()),
                                      static_cast<const uint32_t*>(t[ps_in_RoIs0_next_id].get_dataptr()),
                                      *static_cast<const uint32_t*>(t[ps_in_n_RoIs0].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_RoIs1_dx].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_RoIs1_dy].get_dataptr()),
                                      static_cast<const float*>(t[ps_in_RoIs1_error].get_dataptr()),
                                      static_cast<const uint8_t*>(t[ps_in_RoIs1_is_moving].get_dataptr()),
                                      *static_cast<const uint32_t*>(t[ps_in_n_RoIs1].get_dataptr()));
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_kNN::~Logger_kNN() {
}
