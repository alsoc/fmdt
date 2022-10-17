#include "fmdt/KPPV.h"
#include "fmdt/tools.h"

#include "fmdt/Logger/Logger_KNN.hpp"

Logger_KNN::Logger_KNN(const std::string KNN_path, const size_t i0, const int i1, const int j0, const int j1,
                       const size_t max_ROI_size)
: Module(), KNN_path(KNN_path), i0(i0), i1(i1), j0(j0), j1(j1) {
    const std::string name = "Logger_KNN";
    this->set_name(name);
    this->set_short_name(name);

    this->in_data_nearest = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(uint32_t*)));
    this->in_data_nearest -= i0;
    this->in_data_distances = (const float**)malloc((size_t)(((i1 - i0) + 1) * sizeof(float*)));
    this->in_data_distances -= i0;

    auto &p = this->create_task("write");

    const size_t d_socket_size = ((i1 - i0) + 1) * ((j1 - j0) + 1);
    auto ps_in_data_nearest = this->template create_socket_in<uint32_t>(p, "in_data_nearest", d_socket_size);
    auto ps_in_data_distances = this->template create_socket_in<float>(p, "in_data_distances", d_socket_size);
    // auto ps_in_data_conflicts = this->template create_socket_in<uint32_t>(p, "in_data_conflicts", j1 - j0 + 1);

    auto ps_in_ROI_id = this->template create_socket_in<uint16_t>(p, "in_ROI_id", max_ROI_size);
    auto ps_in_ROI_S = this->template create_socket_in<uint32_t>(p, "in_ROI_S", max_ROI_size);
    auto ps_in_ROI_dx = this->template create_socket_in<float>(p, "in_ROI_dx", max_ROI_size);
    auto ps_in_ROI_dy = this->template create_socket_in<float>(p, "in_ROI_dy", max_ROI_size);
    auto ps_in_ROI_error = this->template create_socket_in<float>(p, "in_ROI_error", max_ROI_size);
    auto ps_in_ROI_next_id = this->template create_socket_in<int32_t>(p, "in_ROI_next_id", max_ROI_size);;
    auto ps_in_n_ROI = this->template create_socket_in<uint32_t>(p, "in_n_ROI", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!KNN_path.empty())
        tools_create_folder(KNN_path.c_str());

    this->create_codelet(p, [ps_in_data_nearest, ps_in_data_distances, ps_in_ROI_id, ps_in_ROI_S, ps_in_ROI_dx,
                             ps_in_ROI_dy, ps_in_ROI_error, ps_in_ROI_next_id, ps_in_n_ROI, ps_in_frame]
                         (aff3ct::module::Module &m, aff3ct::runtime::Task &t, const size_t frame_id) -> int {
        auto &lgr_knn = static_cast<Logger_KNN&>(m);

        const uint32_t* m_in_data_nearest = static_cast<const uint32_t*>(t[ps_in_data_nearest].get_dataptr());
        const float* m_in_data_distances = static_cast<const float*>(t[ps_in_data_distances].get_dataptr());
        lgr_knn.in_data_nearest[lgr_knn.i0] = m_in_data_nearest - lgr_knn.j0;
        lgr_knn.in_data_distances[lgr_knn.i0] = m_in_data_distances - lgr_knn.j0;
        for (int i = lgr_knn.i0 + 1; i <= lgr_knn.i1; i++) {
            lgr_knn.in_data_nearest[i] = lgr_knn.in_data_nearest[i - 1] + ((lgr_knn.j1 - lgr_knn.j0) + 1);
            lgr_knn.in_data_distances[i] = lgr_knn.in_data_distances[i - 1] + ((lgr_knn.j1 - lgr_knn.j0) + 1);
        }

        const uint32_t frame = *static_cast<const size_t*>(t[ps_in_frame].get_dataptr());
        if (frame && !lgr_knn.KNN_path.empty()) {
            char file_path[256];
            sprintf(file_path, "%s/%05u_%05u.txt", lgr_knn.KNN_path.c_str(), frame -1, frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            _KPPV_asso_conflicts_write(file, lgr_knn.in_data_nearest, lgr_knn.in_data_distances,
                                       static_cast<const uint16_t*>(t[ps_in_ROI_id].get_dataptr()),
                                       static_cast<const uint32_t*>(t[ps_in_ROI_S].get_dataptr()),
                                       static_cast<const float*>(t[ps_in_ROI_dx].get_dataptr()),
                                       static_cast<const float*>(t[ps_in_ROI_dy].get_dataptr()),
                                       static_cast<const float*>(t[ps_in_ROI_error].get_dataptr()),
                                       static_cast<const int32_t*>(t[ps_in_ROI_next_id].get_dataptr()),
                                       *static_cast<const uint32_t*>(t[ps_in_n_ROI].get_dataptr()));
            fclose(file);
        }
        return aff3ct::runtime::status_t::SUCCESS;
    });
}

Logger_KNN::~Logger_KNN() {
    free(this->in_data_nearest + this->i0);
    free(this->in_data_distances + this->i0);
}
