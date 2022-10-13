#pragma once

#include <stdint.h>
#include <aff3ct_core.hpp>

namespace lgr_fra {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_img, in_frame, status };
    }
}

class Logger_frame : public aff3ct::module::Module {
protected:
    const std::string frames_path;
    const int i0;
    const int i1;
    const int j0;
    const int j1;
    const int b;
    const uint8_t** in_img;
public:
    Logger_frame(const std::string frames_path, const int i0, const int i1, const int j0, const int j1, const int b);
    virtual ~Logger_frame();
    inline aff3ct::module::Task& operator[](const lgr_fra::tsk t);
    inline aff3ct::module::Socket& operator[](const lgr_fra::sck::write s);
};

#include "fmdt/Logger/Logger_frame.hxx"
