#pragma once

#include <stdint.h>
#include <aff3ct-core.hpp>

namespace lgr_fra {
    enum class tsk : size_t { write, SIZE };
    namespace sck {
        enum class write : size_t { in_labels, in_frame, status };
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
    const uint32_t** in_labels;
    uint8_t** img_tmp;
public:
    Logger_frame(const std::string frames_path, const int i0, const int i1, const int j0, const int j1, const int b);
    virtual ~Logger_frame();
    inline aff3ct::runtime::Task& operator[](const lgr_fra::tsk t);
    inline aff3ct::runtime::Socket& operator[](const lgr_fra::sck::write s);
};

#include "fmdt/Logger/Logger_frame.hxx"
