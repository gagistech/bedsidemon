#pragma once

#include <tml/tree.hpp>

namespace bedsidemon{

struct settings{
    uint32_t sweep_speed_um_per_sec = 25000;
};

class settings_storage{
    std::string filename;

    settings settings_v;

    static settings read(std::string_view filename);
    void write();

public:
    settings_storage();
    ~settings_storage(){
        this->write();// TODO: remove?
    }

    const settings& get()const noexcept{
        return this->settings_v;
    }

    void set(const settings& s);
};

}
