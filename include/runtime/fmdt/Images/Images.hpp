#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

#include "fmdt/images.h"

namespace img {
    enum class tsk : size_t { generate, SIZE };
    namespace sck {
        enum class generate : size_t { out_img, out_frame, status };
    }
}

class Images : public aff3ct::module::Module, public aff3ct::tools::Interface_is_done {
protected:
    int b;
    images_t* images;
    uint8_t** out_img;
    bool done;
public:
    Images(const std::string filename, const size_t frame_start, const size_t frame_end, const size_t frame_skip,
           const int b);
    virtual ~Images();
    virtual bool is_done() const;
    inline uint8_t** get_out_img();
    inline int get_i0();
    inline int get_i1();
    inline int get_j0();
    inline int get_j1();
    inline int get_b();
    inline aff3ct::runtime::Task& operator[](const img::tsk t);
    inline aff3ct::runtime::Socket& operator[](const img::sck::generate s);
};

#include "fmdt/Images/Images.hxx"
