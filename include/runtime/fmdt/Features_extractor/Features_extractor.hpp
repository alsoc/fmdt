#pragma once

#include <stdint.h>
#include <aff3ct.hpp>

#include "fmdt/features.h"

namespace ftr_ext {
    enum class tsk : size_t { extract, SIZE };
    namespace sck {
        enum class extract : size_t { in_img, in_n_ROI, out_ROI_id, out_ROI_xmin, out_ROI_xmax, out_ROI_ymin,
                                      out_ROI_ymax, out_ROI_S, out_ROI_Sx, out_ROI_Sy, out_ROI_x, out_ROI_y, status };
    }
}

class Features_extractor : public aff3ct::module::Module {
protected:
    const int i0, i1, j0, j1;
    const int b;
    const size_t max_ROI_size;
    const uint32_t** in_img;
public:
    Features_extractor(const int i0, const int i1, const int j0, const int j1, const int b, const size_t max_ROI_size)
    : Module(), i0(i0), i1(i1), j0(j0), j1(j1), b(b), max_ROI_size(max_ROI_size), in_img(nullptr) {
        const std::string name = "Features_extractor";
        this->set_name(name);
        this->set_short_name(name);

        this->in_img = (const uint32_t**)malloc((size_t)(((i1 - i0) + 1 + 2 * b) * sizeof(const uint32_t*)));
        this->in_img -= i0 - b;

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
            auto &ext = static_cast<Features_extractor&>(m);
            const uint32_t* m_in_img = static_cast<const uint32_t*>(t[ps_in_img].get_dataptr());
            ext.in_img[ext.i0 - ext.b] = m_in_img - (ext.j0 - ext.b);
            for (int i = ext.i0 - ext.b + 1; i <= ext.i1 + ext.b; i++)
                ext.in_img[i] = ext.in_img[i - 1] + ((ext.j1 - ext.j0) + 1 + 2 * ext.b);

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

    virtual ~Features_extractor() {
        free(this->in_img + (this->i0 - this->b));
    }

    inline aff3ct::module::Task& operator[](const ftr_ext::tsk t) {
        return aff3ct::module::Module::operator[]((size_t)t);
    }

    inline aff3ct::module::Socket& operator[](const ftr_ext::sck::extract s) {
        return aff3ct::module::Module::operator[]((size_t)ftr_ext::tsk::extract)[(size_t)s];
    }
};
