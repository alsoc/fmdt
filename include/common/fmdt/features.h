#pragma once

#include <stdint.h>

#include "fmdt/tracking.h"

typedef struct {
    uint16_t* id;
    size_t* frame;
    uint16_t* xmin;
    uint16_t* xmax;
    uint16_t* ymin;
    uint16_t* ymax;
    uint32_t* S; // number of points
    uint32_t* Sx; // sum of x properties
    uint32_t* Sy; // sum of y properties
    float* x; // abscisse du centre d'inertie x = Sx / S
    float* y; // ordonnee du centre d'inertie y = Sy / S
    float* dx; // erreur par rapport a l`image recalee
    float* dy; // erreur par rapport a l`image recalee
    float* error;
    int32_t* time;
    int32_t* time_motion;
    int32_t* prev_id; // associated CC from t-1 -> t -> t+1
    int32_t* next_id; // associated CC from t-1 -> t -> t+1
    uint8_t* is_moving;
    uint8_t* is_extrapolated;

    size_t _size; // current size/utilization of the fields
    size_t _max_size; // maximum amount of data that can be contained in the fields
} ROI_t;

// defined in "tracking.h"
typedef struct track track_t;

ROI_t* features_alloc_ROI_array(const size_t max_size);
void features_init_ROI_array(ROI_t* ROI_array);
void features_free_ROI_array(ROI_t* ROI_array);
void features_clear_index_ROI_array(ROI_t* ROI_array, const size_t r);
void features_copy_elmt_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest, const int i_src, const int i_dest);
void features_copy_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest);
void features_init_ROI(ROI_t* stats, int n);
void _features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, uint16_t* ROI_id,
                       uint16_t* ROI_xmin, uint16_t* ROI_xmax, uint16_t* ROI_ymin, uint16_t* ROI_ymax, uint32_t* ROI_S,
                       uint32_t* ROI_Sx, uint32_t* ROI_Sy, float* ROI_x, float* ROI_y, const size_t n_ROI);
void features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, const size_t n_ROI,
                      ROI_t* ROI_array);
// void features_filter_surface(ROI_t* ROI_array, uint32_t** img, uint32_t threshold_min, uint32_t threshold_max);
void features_merge_HI_CCL_v2(const uint32_t** M, const uint8_t** HI_in, uint8_t** HI_out, const int i0, const int i1,
                              const int j0, const int j1, ROI_t* ROI_array, const uint32_t S_min, const uint32_t S_max);
void features_shrink_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest);
double features_compute_mean_error(const ROI_t* stats);
double features_compute_std_deviation(const ROI_t* stats, const double mean_error);
void features_compute_motion(const ROI_t* ROI_array1, ROI_t* ROI_array0, double* theta, double* tx,
                             double* ty, double* mean_error, double* std_deviation);
// void features_motion_extraction(ROI_t* stats0, ROI_t* stats1, int nc0, double theta, double tx, double ty);
// void features_print_stats(ROI_t* stats, int n);
// void features_parse_stats(const char* filename, ROI_t* stats, int* n);
// void features_save_stats(const char* filename, const ROI_t* ROI_array, const track_array_t* track_array,
//                          const unsigned age);
void features_save_stats_file(FILE* f, const ROI_t* ROI_array, const track_t* track_array, const unsigned age);
// void features_save_stats(const char* filename, ROI_t* stats, int n, track_t* tracks);
// void features_save_motion(const char* filename, double theta, double tx, double ty, int frame);
// void features_save_error(const char* filename, const ROI_t* ROI_array);
// void features_save_error_moy(const char* filename, double mean_error, double std_deviation);
// void features_save_motion_extraction(const char* filename, const ROI_array_t* ROI_array, const double theta,
//                                      const double tx, const double ty, const int frame);

#if defined(__cplusplus) && defined(AFF3CT_LINK)

#include <aff3ct.hpp>

namespace ftr {

enum class tsk : size_t { extract, SIZE };
namespace sck {
    enum class extract : size_t { in_img, in_n_ROI, out_ROI_id, out_ROI_xmin, out_ROI_xmax, out_ROI_ymin, out_ROI_ymax,
                                  out_ROI_S, out_ROI_Sx, out_ROI_Sy, out_ROI_x, out_ROI_y, status };
}

class Extractor : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const size_t max_ROI_size;
    const uint32_t** in_img;
public:
    Extractor(const int i0, const int i1, const int j0, const int j1, const int b, const size_t max_ROI_size)
    : i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_ROI_size(max_ROI_size), in_img(nullptr) {
        this->in_img = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint32_t*)));

        auto socket_img_size = ((i1 - i0) + 1 + 2 * b) * ((j1 - j0) + 1 + 2 * b);

        auto &p = this->create_task("extract");
        auto ps_in_img = this->template create_socket_in<uint32_t>(p, "in_img", socket_img_size);
        auto ps_in_n_ROI = this->template create_socket_in<uint32_t>(p, "in_n_ROI", 1);
        auto ps_out_ROI_id = this->template create_socket_out<uint16_t>(p, "out_ROI_id", max_ROI_size);
        auto ps_out_ROI_xmin = this->template create_socket_out<uint16_t>(p, "out_ROI_xmin", max_ROI_size);
        auto ps_out_ROI_xmax = this->template create_socket_out<uint16_t>(p, "out_ROI_xmax", max_ROI_size);
        auto ps_out_ROI_ymin = this->template create_socket_out<uint16_t>(p, "out_ROI_ymin", max_ROI_size);
        auto ps_out_ROI_ymax = this->template create_socket_out<uint16_t>(p, "out_ROI_ymax", max_ROI_size);
        auto ps_out_ROI_S = this->template create_socket_out<uint32_t>(p, "out_ROI_S", max_ROI_size);
        auto ps_out_ROI_Sx = this->template create_socket_out<uint32_t>(p, "out_ROI_Sx", max_ROI_size);
        auto ps_out_ROI_Sy = this->template create_socket_out<uint32_t>(p, "out_ROI_Sy", max_ROI_size);
        auto ps_out_ROI_x = this->template create_socket_out<float>(p, "out_ROI_x", max_ROI_size);
        auto ps_out_ROI_y = this->template create_socket_out<float>(p, "out_ROI_y", max_ROI_size);

        this->create_codelet(p, [ps_in_img, ps_in_n_ROI, ps_out_ROI_id, ps_out_ROI_xmin, ps_out_ROI_xmax,
                                 ps_out_ROI_ymin, ps_out_ROI_ymax, ps_out_ROI_S, ps_out_ROI_Sx, ps_out_ROI_Sy,
                                 ps_out_ROI_x, ps_out_ROI_y](aff3ct::module::Module &m, aff3ct::module::Task &t,
                                                             const size_t frame_id) -> int {
            auto &ext = static_cast<Extractor&>(m);
            const uint32_t* m_in_img = static_cast<const uint32_t*>(t[ps_in_img].get_dataptr());
            for (auto i = ext.i0 - ext.b; i <= ext.i1 + ext.b; i++)
                ext.in_img[i] = m_in_img + i * ((ext.j1 - ext.j0) + 1 + 2 * ext.b);

            uint32_t n_ROI = *static_cast<uint32_t*>(t[ps_in_n_ROI].get_dataptr());

            _features_extract(ext.in_img, ext.i0, ext.i1, ext.j0, ext.j1,
                              static_cast<uint16_t*>(t[ps_out_ROI_id].get_dataptr()),
                              static_cast<uint16_t*>(t[ps_out_ROI_xmin].get_dataptr()),
                              static_cast<uint16_t*>(t[ps_out_ROI_xmax].get_dataptr()),
                              static_cast<uint16_t*>(t[ps_out_ROI_ymin].get_dataptr()),
                              static_cast<uint16_t*>(t[ps_out_ROI_ymax].get_dataptr()),
                              static_cast<uint32_t*>(t[ps_out_ROI_S].get_dataptr()),
                              static_cast<uint32_t*>(t[ps_out_ROI_Sx].get_dataptr()),
                              static_cast<uint32_t*>(t[ps_out_ROI_Sy].get_dataptr()),
                              static_cast<float*>(t[ps_out_ROI_x].get_dataptr()),
                              static_cast<float*>(t[ps_out_ROI_y].get_dataptr()),
                              n_ROI);

            return aff3ct::module::status_t::SUCCESS;
        });
    }

    virtual ~Extractor() {
        free(this->in_img);
    }

    inline aff3ct::module::Task& operator[](const ftr::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const ftr::sck::extract s) {
        return aff3ct::module::Module::operator[]((size_t)ftr::tsk::extract)[(size_t)s];
    }
};

}

#endif
