#pragma once

#include <stdint.h>

void threshold(const uint8_t** m_in, uint8_t** m_out, const int i0, const int i1, const int j0, const int j1,
               const uint8_t threshold);
void threshold_low(const uint8_t** m_in, uint8_t** m_out, const int i0, const int i1, const int j0, const int j1,
                   const uint8_t threshold);
void threshold_high(const uint8_t** m_in, uint8_t** m_out, const int i0, const int i1, const int j0, const int j1,
                    const uint8_t threshold);
// float max_norme(float** U, float** V, int i0, int i1, int j0, int j1);
// void threshold_norme_compact_bigend(float** U, float** V, uint8_t** out, int w, int h, float threshold);
// void threshold_compact_bigend(uint8_t** in, uint8_t** out, int w, int h, uint8_t threshold);
// void threshold_norme_littleend(float** U, float** V, uint8_t** out, int i0, int i1, int j0, int j1, float threshold);
// void threshold_norme_compact_littleend(float** U, float** V, uint8_t** out, int w, int h, float threshold);
// void pack_ui8vector_bigend(uint8_t* X1, int src_width, uint8_t* Y8);
// void pack_ui8matrix_bigend(uint8_t** X1, uint8_t** Y8, int src_width, int src_height);
// void pack_ui8vector_littleend(uint8_t* X1, int src_width, uint8_t* Y8);
// void pack_ui8matrix_littleend(uint8_t** X1, int src_height, int src_width, uint8_t** Y8);
// void pack255_ui8vector_littleend(uint8_t* X1, int src_width, uint8_t* Y8);
// void pack255_ui8matrix_littleend(uint8_t** X1, int src_height, int src_width, uint8_t** Y8);
// void unpack_ui8matrix_bigend(uint8_t** in, uint8_t** out, int w, int h);
// void unpack_ui32matrix_bigend(uint8_t** in, uint32_t** out, int w, int h);
// void unpack255_ui8matrix_littleend(uint8_t** in, uint8_t** out, int w, int h);
// void unpack_ui8vector_littleend(uint8_t* X8, int src_width, uint8_t* Y1);
// void unpack_ui8matrix_littleend(uint8_t** X8, int src_height, int src_width, uint8_t** Y1);
// void unpack_ui32matrix_littlend(uint8_t** in, uint32_t** out, int w, int h);
// void mask_EDT(uint8_t** I1, uint8_t** I2, int i0, int i1, int j0, int j1);
// void mask_EDT_ui32matrix(uint32_t** I1, uint32_t** I2, int i0, int i1, int j0, int j1);
// void mask_EDT_compact(uint8_t** I1, uint8_t** I2, int i0, int i1, int j0, int j1);
// void histogram(uint8_t** m, float* h, int i0, int i1, int j0, int j1);
// void histogram_uv_norm_sq(uint8_t** m, float** U, float** V, float* h, int i0, int i1, int j0, int j1);
// int otsu_bcv_k(float* h, int k);
// int otsu_bcv(float* h, int t0, int t1);
// int otsu_wcv(uint8_t** m, int i0, int i1, int j0, int j1);

// OpenMP
// void unpack_ui8matrix_omp(uint8_t** in, uint8_t** out, int w, int h);
// void unpack_ui32matrix_omp(uint8_t** in, uint32_t** out, int w, int h);

#if defined(__cplusplus) && defined(AFF3CT_LINK)

#include <aff3ct.hpp>

namespace thr {

enum class tsk : size_t { apply, SIZE };
namespace sck {
    enum class apply : size_t { in_img, out_img, status };
}

class Threshold : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const uint8_t thr_val;
    const uint8_t** in_img;
    uint8_t** out_img;
public:
    Threshold(const int i0, const int i1, const int j0, const int j1, const uint8_t thr_val)
    : i0(i0), i1(i1), j0(j0), j1(j1), thr_val(thr_val), in_img(nullptr), out_img(nullptr) {
        this->in_img = (const uint8_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(const uint8_t*)));
        this->out_img = (uint8_t**)malloc((size_t)(((i1 - i0) + 1) * sizeof(uint8_t*)));
        this->in_img -= i0;
        this->out_img -= i0;

        auto socket_size = ((i1 - i0) + 1) * ((j1 - j0) + 1);

        auto &p = this->create_task("apply");
        auto ps_in_img = this->template create_socket_in<uint8_t>(p, "in", socket_size);
        auto ps_out_img = this->template create_socket_out<uint8_t>(p, "out", socket_size);

        this->create_codelet(p, [ps_in_img, ps_out_img](aff3ct::module::Module &m, aff3ct::module::Task &t,
                             const size_t frame_id) -> int {
            auto &thr = static_cast<Threshold&>(m);
            const uint8_t* m_in_img = static_cast<const uint8_t*>(t[ps_in_img].get_dataptr());
            uint8_t* m_out_img = static_cast<uint8_t*>(t[ps_out_img].get_dataptr());
            for (auto i = thr.i0; i <= thr.i1; i++) {
                thr.in_img[i] = m_in_img + (i - thr.i0) * ((thr.j1 - thr.j0) + 1);
                thr.in_img[i] -= thr.j0;
            }

            thr.out_img[thr.i0] = m_out_img;
            for (int i = thr.i0 +1; i <= thr.i1; i++) {
                thr.out_img[i] = thr.out_img[i - 1] + ((thr.j1 - thr.j0) + 1);
            }

            threshold(thr.in_img, thr.out_img, thr.i0, thr.i1, thr.j0, thr.j1, thr.thr_val);
            return aff3ct::module::status_t::SUCCESS;
        });
    }

    virtual ~Threshold() {
        free(this->in_img + i0);
        free(this->out_img + i0);
    }

    inline uint8_t** get_out_img() {
        return this->out_img;
    }

    inline aff3ct::module::Task& operator[](const thr::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const thr::sck::apply s) {
        return aff3ct::module::Module::operator[]((size_t)thr::tsk::apply)[(size_t)s];
    }
};

}

#endif