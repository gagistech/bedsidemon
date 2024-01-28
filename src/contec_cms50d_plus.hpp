#pragma once

#include <nitki/loop_thread.hpp>

namespace bedsidemon{

class contec_cms50d_plus : private nitki::loop_thread{
public:
    contec_cms50d_plus();
    ~contec_cms50d_plus();

private:
    std::optional<uint32_t> on_loop()override;
};

}
