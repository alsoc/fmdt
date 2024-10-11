#include "fmdt/kNN/kNN_io.h"
#include "fmdt/tools.h"

#include "fmdt/spu/Logger/Logger_kNN.hpp"

Logger_kNN::Logger_kNN(const std::string kNN_path, const size_t fra_start, const size_t max_size)
: spu::module::Stateful(), kNN_path(kNN_path), fra_start(fra_start), max_size(max_size) {
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
    auto ps_in_RoIs0_basic = this->template create_socket_in<uint8_t>(p, "in_RoIs0_basic",
                                                                      max_size * sizeof(RoI_basic_t));
    auto ps_in_RoIs0_asso = this->template create_socket_in<uint8_t>(p, "in_RoIs0_asso",
                                                                     max_size * sizeof(RoI_asso_t));
    auto ps_in_n_RoIs0 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs0", 1);
    auto ps_in_RoIs1_motion = this->template create_socket_in<uint8_t>(p, "in_RoIs1_motion",
                                                                       max_size * sizeof(RoI_motion_t));
    auto ps_in_n_RoIs1 = this->template create_socket_in<uint32_t>(p, "in_n_RoIs1", 1);
    auto ps_in_frame = this->template create_socket_in<uint32_t>(p, "in_frame", 1);

    if (!kNN_path.empty())
        tools_create_folder(kNN_path.c_str());

#ifdef FMDT_ENABLE_DEBUG
    this->create_codelet(p, [ps_in_data_nearest, ps_in_data_distances, ps_in_data_conflicts, ps_in_RoIs0_basic,
                             ps_in_RoIs0_asso, ps_in_n_RoIs0, ps_in_RoIs1_motion, ps_in_n_RoIs1, ps_in_frame]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
#else
    this->create_codelet(p, [ps_in_data_nearest, ps_in_data_distances, ps_in_RoIs0_basic, ps_in_RoIs0_asso,
                             ps_in_n_RoIs0, ps_in_RoIs1_motion, ps_in_n_RoIs1, ps_in_frame]
                         (spu::module::Module &m, spu::runtime::Task &t, const size_t frame_id) -> int {
#endif
        auto &lgr_knn = static_cast<Logger_kNN&>(m);

        // calling get_2d_dataptr() has a small overhead (it performs the 1D to 2D conversion)
        const float**       in_data_distances =  t[ps_in_data_distances].get_2d_dataptr<const float       >();
        const uint32_t**    in_data_nearest   =  t[ps_in_data_nearest  ].get_2d_dataptr<const uint32_t    >();
#ifdef FMDT_ENABLE_DEBUG
        const uint32_t*     in_data_conflicts =  t[ps_in_data_conflicts].get_dataptr   <const uint32_t    >();
#endif
        const uint32_t      in_frame          = *t[ps_in_frame         ].get_dataptr   <const uint32_t    >();
        const RoI_basic_t*  in_RoIs0_basic    =  t[ps_in_RoIs0_basic   ].get_dataptr   <const RoI_basic_t >();
        const RoI_asso_t*   in_RoIs0_asso     =  t[ps_in_RoIs0_asso    ].get_dataptr   <const RoI_asso_t  >();
        const uint32_t      in_n_RoIs0        = *t[ps_in_n_RoIs0       ].get_dataptr   <const uint32_t    >();
        const RoI_motion_t* in_RoIs1_motion   =  t[ps_in_RoIs1_motion  ].get_dataptr   <const RoI_motion_t>();
        const uint32_t      in_n_RoIs1        = *t[ps_in_n_RoIs1       ].get_dataptr   <const uint32_t    >();

#ifndef FMDT_ENABLE_DEBUG
        const uint32_t* in_data_conflicts = nullptr;
#endif

        if (in_frame > (uint32_t)lgr_knn.fra_start && !lgr_knn.kNN_path.empty()) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%05u.txt", lgr_knn.kNN_path.c_str(), in_frame);
            FILE* file = fopen(file_path, "a");
            fprintf(file, "#\n");
            kNN_data_t kNN_data = { (float**)in_data_distances, (uint32_t**)in_data_nearest,
                                    (uint32_t*)in_data_conflicts, lgr_knn.max_size };
            kNN_asso_conflicts_write(file, &kNN_data, in_RoIs0_basic, in_RoIs0_asso, in_n_RoIs0, in_RoIs1_motion,
                                     in_n_RoIs1);
            fclose(file);
        }
        return spu::runtime::status_t::SUCCESS;
    });
}

Logger_kNN::~Logger_kNN() {
}
